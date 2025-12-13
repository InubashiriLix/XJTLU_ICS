#!/usr/bin/env bash
# Test script to verify redirect controller optimization
# Measures connection delay and verifies performance improvement

set -euo pipefail

BASE_DIR="$(cd "$(dirname "$0")" && pwd)"
RYU_PORT="${RYU_PORT:-6633}"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo "╔════════════════════════════════════════════════════════════════╗"
echo "║     Redirect Controller Delay Verification Script             ║"
echo "║              Fast-Redirect Target: < 10ms                      ║"
echo "╚════════════════════════════════════════════════════════════════╝"
echo ""

# Root check - auto-escalate with sudo
if [[ $EUID -ne 0 ]]; then
  if command -v sudo >/dev/null 2>&1; then
    echo -e "${YELLOW}[Notice] Root privileges required. Re-executing with sudo...${NC}"
    exec sudo -E env "PATH=$PATH" "$0" "$@"
  else
    echo -e "${RED}Error: This script requires root privileges.${NC}"
    echo "Please run: sudo $0"
    exit 1
  fi
fi

# Check dependencies
for cmd in mn ryu-manager python3 ovs-ofctl; do
  command -v $cmd >/dev/null || { echo "Error: $cmd not found"; exit 1; }
done

# Cleanup function
cleanup() {
  echo ""
  echo -e "${YELLOW}[Cleanup] Stopping Mininet and Ryu...${NC}"
  mn -c >/dev/null 2>&1 || true
  pkill -f ryu-manager >/dev/null 2>&1 || true
  sleep 1
}
trap cleanup EXIT

# Start Ryu controller
start_controller() {
  local log="/tmp/test_ryu_redirect.log"
  echo -e "${BLUE}[1/5] Starting Ryu controller...${NC}"
  : > "${log}"
  
  RYU_PORT="${RYU_PORT}" ryu-manager --ofp-tcp-listen-port "${RYU_PORT}" \
    "${BASE_DIR}/ryu_redirect.py" >"${log}" 2>&1 &
  
  local pid=$!
  sleep 2
  
  if ! kill -0 "${pid}" >/dev/null 2>&1; then
    echo -e "${RED}Error: Ryu failed to start. Check ${log}${NC}"
    cat "${log}"
    exit 1
  fi
  
  echo -e "${GREEN}✓ Ryu controller started (PID: ${pid})${NC}"
  echo ""
}

# Test function that runs client and measures delay
test_redirect_delay() {
  local test_name="$1"
  local iterations="${2:-3}"
  
  echo -e "${BLUE}[2/5] Starting Mininet topology...${NC}"
  
  # Start network and measure delays
  python3 - "${BASE_DIR}" "${iterations}" <<'PYTEST'
import sys
import time
import os
BASE = sys.argv[1]
ITERATIONS = int(sys.argv[2])

sys.path.insert(0, BASE)
from networkTopo import build_network
from mininet.log import setLogLevel

print("\033[92m✓ Mininet topology started\033[0m")
print("")

setLogLevel('error')
net = build_network()

try:
    net.start()
    
    # Wait for controller connection
    time.sleep(2)
    
    print("\033[94m[3/5] Checking OpenFlow flows...\033[0m")
    os.system("sudo ovs-ofctl dump-flows s1 -O OpenFlow13 | grep 'priority=100'")
    flow_count = os.popen("sudo ovs-ofctl dump-flows s1 -O OpenFlow13 | grep -c 'priority=100'").read().strip()
    
    if flow_count == "2":
        print("\033[92m✓ Proactive flows installed (2 flows with priority=100)\033[0m")
    else:
        print("\033[93m⚠ Warning: Expected 2 proactive flows, found {}\033[0m".format(flow_count))
    print("")
    
    # Start servers
    print("\033[94m[4/5] Starting servers...\033[0m")
    net.get("server1").cmd(f"python3 {BASE}/server.py 0.0.0.0 5000 > /tmp/server1.log 2>&1 &")
    net.get("server2").cmd(f"python3 {BASE}/server.py 0.0.0.0 5000 > /tmp/server2.log 2>&1 &")
    time.sleep(1)
    print("\033[92m✓ Servers started on server1 and server2\033[0m")
    print("")
    
    # Run tests
    print("\033[94m[5/5] Testing redirect connection ({} iterations)...\033[0m".format(ITERATIONS))
    print("\033[90m" + "─" * 70 + "\033[0m")
    
    delays = []
    client_delays = []
    
    for i in range(ITERATIONS):
        start = time.perf_counter()
        output = net.get("client").cmd(f"python3 {BASE}/client.py 10.0.1.2 5000 2>&1")
        end = time.perf_counter()
        
        delay = (end - start) * 1000  # Convert to milliseconds
        delays.append(delay)
        
        # Check if connection was successful
        if "Finished in" in output and "Received ACK" in output:
            status = "\033[92m✓ SUCCESS\033[0m"
            # Extract actual client-reported time
            client_time_ms = None
            for line in output.split('\n'):
                if "Finished in" in line:
                    try:
                        client_time = float(line.split("Finished in")[1].split("seconds")[0].strip())
                        client_time_ms = client_time * 1000
                    except:
                        pass
                    break
            
            if client_time_ms is not None:
                client_delays.append(client_time_ms)
                if client_time_ms < 10:
                    perf_indicator = "\033[92m⚡\033[0m"  # Lightning for < 10ms
                elif client_time_ms < 50:
                    perf_indicator = "\033[92m✓\033[0m"   # Green check for < 50ms
                elif client_time_ms < 100:
                    perf_indicator = "\033[93m~\033[0m"   # Yellow tilde for < 100ms
                else:
                    perf_indicator = "\033[91m✗\033[0m"   # Red X for >= 100ms
                print(f"  Test {i+1}/{ITERATIONS}: {client_time_ms:7.2f}ms {perf_indicator}")
            else:
                print(f"  Test {i+1}/{ITERATIONS}: {delay:7.2f}ms {status}")
        else:
            status = "\033[91m✗ FAILED\033[0m"
            print(f"  Test {i+1}/{ITERATIONS}: {delay:7.2f}ms {status}")
            print(f"     Output: {output[:100]}")
        
        if i < ITERATIONS - 1:
            time.sleep(0.5)  # Brief pause between tests
    
    print("\033[90m" + "─" * 70 + "\033[0m")
    print("")
    
    # Calculate statistics
    if client_delays:
        avg_delay = sum(client_delays) / len(client_delays)
        min_delay = min(client_delays)
        max_delay = max(client_delays)
        
        print("\033[1m📊 RESULTS (TCP Connection Time):\033[0m")
        print(f"  Average delay: \033[1;36m{avg_delay:.2f}ms\033[0m")
        print(f"  Min delay:     {min_delay:.2f}ms")
        print(f"  Max delay:     {max_delay:.2f}ms")
        print("")
        print("\033[90m  Note: This measures pure TCP connection + data exchange time")
        print("  (from socket.connect() to receiving ACK)")
        print("  Total execution time includes Python startup overhead (~15-20ms)\033[0m")
        print("")
    elif delays:
        avg_delay = sum(delays) / len(delays)
        min_delay = min(delays)
        max_delay = max(delays)
        
        print("\033[1m📊 RESULTS (Total Execution Time):\033[0m")
        print(f"  Average delay: \033[1;36m{avg_delay:.2f}ms\033[0m")
        print(f"  Min delay:     {min_delay:.2f}ms")
        print(f"  Max delay:     {max_delay:.2f}ms")
        print("")
    else:
        print("\033[91mNo valid results\033[0m")
        sys.exit(1)
        
        # Performance assessment - targeting < 10ms
        if avg_delay < 10:
            print("\033[1;32m✓ EXCELLENT: Delay < 10ms (Fast-redirect working perfectly!)\033[0m")
            print("\033[92m  Hardware-speed redirection achieved.\033[0m")
        elif avg_delay < 50:
            print("\033[1;32m✓ VERY GOOD: Delay < 50ms (Fast-redirect working!)\033[0m")
            print("\033[92m  Proactive flows eliminating TCP retransmission.\033[0m")
        elif avg_delay < 100:
            print("\033[1;33m⚠ GOOD: Delay < 100ms (Acceptable)\033[0m")
            print("\033[93m  Proactive flows working but could be optimized.\033[0m")
        elif avg_delay < 500:
            print("\033[1;33m⚠ WARNING: Delay 100-500ms (Suboptimal)\033[0m")
            print("\033[93m  Check if proactive flows are installed correctly.\033[0m")
        elif avg_delay < 1000:
            print("\033[1;31m✗ POOR: Delay 500-1000ms (Optimization failing)\033[0m")
            print("\033[91m  Possible issue with flow installation.\033[0m")
        else:
            print("\033[1;31m✗ VERY POOR: Delay > 1000ms (Not optimized)\033[0m")
            print("\033[91m  Likely still using reactive flow installation.\033[0m")
            print("\033[91m  TCP retransmission detected!\033[0m")
        print("")
        
        # Additional diagnostics
        print("\033[1m🔍 DIAGNOSTICS:\033[0m")
        
        # Check packet capture hint
        print("  To verify with packet capture:")
        print("    mininet> client tcpdump -i client-eth0 -nn 'port 5000' -w /tmp/redirect.pcap &")
        print("    mininet> client python3 client.py 10.0.1.2 5000")
        print("    mininet> exit")
        print("    $ tcpdump -r /tmp/redirect.pcap -nn -tttt | grep 'Flags \\[S\\]'")
        print("")
        
        # Expected behavior
        if avg_delay < 100:
            print("  Expected: Only 1 SYN packet (no retransmission)")
        else:
            print("  Expected: 2 SYN packets (1 original + 1 retransmission)")
        print("")
        
        # Return exit code based on performance
        if avg_delay < 100:
            sys.exit(0)  # Success
        else:
            sys.exit(1)  # Failed to meet performance target
    
finally:
    net.stop()
    print("\033[90mMininet stopped.\033[0m")

PYTEST
}

# Main execution
echo -e "${YELLOW}Testing redirect controller optimization...${NC}"
echo ""

start_controller

# Run test with 5 iterations
test_redirect_delay "Redirect Delay Test" 5

TEST_RESULT=$?

echo ""
echo "╔════════════════════════════════════════════════════════════════╗"
echo "║                    Test Complete                               ║"
echo "╚════════════════════════════════════════════════════════════════╝"
echo ""

if [ $TEST_RESULT -eq 0 ]; then
  echo -e "${GREEN}✓ SUCCESS: Fast-redirect optimization is working!${NC}"
  echo -e "${GREEN}  Average delay < 100ms achieved.${NC}"
else
  echo -e "${RED}✗ FAILED: Delay target not met.${NC}"
  echo -e "${RED}  Average delay >= 100ms. Check flow installation.${NC}"
fi
echo ""

echo "Logs available at:"
echo "  - Controller: /tmp/test_ryu_redirect.log"
echo "  - Server1:    /tmp/server1.log"
echo "  - Server2:    /tmp/server2.log"
echo ""

exit $TEST_RESULT
