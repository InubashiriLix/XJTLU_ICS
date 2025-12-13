#!/usr/bin/env bash
# CAN201 Part II helper runner.
# Usage:
#   ./run_tests.sh                # run all Task1–Task5 sequentially
#   ./run_tests.sh task4          # run a specific task
#   ./run_tests.sh task4 task5    # run selected tasks in order
#
# Tasks mapped:
#   task1: Topology build smoke (networkTopo.py)
#   task2: Basic SDN ping (ryu_forward.py used as controller)
#   task3: Socket app after flows expire (ryu_forward.py, wait 5s post-ping)
#   task4: Forwarding controller functional test (ryu_forward.py)
#   task5: Redirecting controller functional test (ryu_redirect.py)

set -euo pipefail

BASE_DIR="$(cd "$(dirname "$0")" && pwd)"
RYU_PORT="${RYU_PORT:-6633}"
TASKS=("$@")
[[ ${#TASKS[@]} -eq 0 ]] && TASKS=(task1 task2 task3 task4 task5)

usage() {
  cat <<'EOF'
Usage: ./run_tests.sh [task1|task2|task3|task4|task5|all]
  task1: 拓扑搭建 smoke test
  task2: 基础 SDN 控制（ping）
  task3: socket 客户端/服务端（先 ping，等待 idle_timeout 过期，再传输）
  task4: 正常转发 Forwarding
  task5: 重定向 Redirecting
  all  : 依次运行 task1–task5
可同时指定多个任务，例如：./run_tests.sh task4 task5
EOF
}

if [[ "${TASKS[0]}" == "all" ]]; then
  TASKS=(task1 task2 task3 task4 task5)
fi

# Root / dependency checks
if [[ $EUID -ne 0 ]]; then
  if command -v sudo >/dev/null 2>&1; then
    sudo -n true 2>/dev/null || {
      echo "Please run as root: sudo -E env \"PATH=$PATH\" $0 $*"
      exit 1
    }
    exec sudo -E env "PATH=$PATH" "$0" "$@"
  else
    echo "Please run this script as root."
    exit 1
  fi
fi

command -v mn >/dev/null || { echo "mininet (mn) not found"; exit 1; }
command -v ryu-manager >/dev/null || { echo "ryu-manager not found"; exit 1; }
command -v python3 >/dev/null || { echo "python3 not found"; exit 1; }

cleanup() { mn -c >/dev/null 2>&1 || true; }
trap cleanup EXIT

start_ryu() {
  local app="$1" log="$2"
  # Print human-readable notice to stderr so stdout can safely carry only the PID.
  echo "=== 启动 Ryu 控制器 ${app} (log: ${log}) ===" >&2
  # Pre-create/truncate log file to verify path and permissions.
  : >"${log}"
  # run ryu-manager in the background but surface startup errors immediately.
  set +e
  RYU_PORT="${RYU_PORT}" ryu-manager --ofp-tcp-listen-port "${RYU_PORT}" "${BASE_DIR}/${app}" >"${log}" 2>&1 &
  local rc=$?
  local pid=$!
  set -e
  if [[ ${rc} -ne 0 ]]; then
    echo "Ryu 启动失败（ryu-manager exit=${rc}）；查看日志 ${log}"
    cat "${log}"
    exit 1
  fi
  sleep 1
  kill -0 "${pid}" >/dev/null 2>&1 || { echo "Ryu failed; see ${log}"; cat "${log}"; exit 1; }
  echo "${pid}"
}

stop_ryu() {
  local pid="$1" app="$2"
  echo "=== 停止 Ryu (${app}) ==="
  kill "${pid}" >/dev/null 2>&1 || true
  sleep 1
}

run_mininet_block() {
  # Args: mode_label wait_after_ping run_apps
  local label="$1" wait_after_ping="$2" run_apps="$3"
  python3 - "${BASE_DIR}" "${label}" "${wait_after_ping}" "${run_apps}" <<'PY'
import sys, time, traceback
BASE, label, wait_after, run_apps = sys.argv[1], sys.argv[2], float(sys.argv[3]), sys.argv[4] == "1"
sys.path.insert(0, BASE)
from networkTopo import build_network
from mininet.log import setLogLevel

print(f"[mn] Task label: {label}")
setLogLevel('error')
net = build_network()
try:
    net.start()
    print("[mn] pingAll ->", net.pingAll())
    if wait_after > 0:
        print(f"[mn] 等待 {wait_after}s 让流表过期 (idle_timeout=5)")
        time.sleep(wait_after)
    if run_apps:
        for h in ("server1", "server2"):
            net.get(h).cmd(f"python3 {BASE}/server.py 0.0.0.0 5000 > /tmp/{h}.log 2>&1 &")
        time.sleep(0.5)
        print("[mn] client -> 10.0.1.2:5000")
        output = net.get("client").cmd(f"python3 {BASE}/client.py 10.0.1.2 5000")
        print(output)
finally:
    try:
        net.stop()
    except Exception:
        traceback.print_exc()
PY
}

run_task1() {
  echo "=== Task1: 拓扑搭建（networkTopo.py smoke）==="
  echo "Info: 仅启动/停止拓扑并运行自带 pingAll，未启动控制器。"
  python3 "${BASE_DIR}/networkTopo.py" || { echo "Task1 failed"; exit 1; }
}

run_task2() {
  echo "=== Task2: 基础 SDN 控制（ping + idle_timeout=5）==="
  local log="/tmp/task2_ryu_forward.log"
  local pid
  pid=$(start_ryu "ryu_forward.py" "${log}")
  run_mininet_block "task2" 0 0
  stop_ryu "${pid}" "ryu_forward.py"
}

run_task3() {
  echo "=== Task3: socket 测试（先 ping，再等 5s，让 flow 过期，再传输）==="
  local log="/tmp/task3_ryu_forward.log"
  local pid
  pid=$(start_ryu "ryu_forward.py" "${log}")
  run_mininet_block "task3" 5 1
  stop_ryu "${pid}" "ryu_forward.py"
}

run_task4() {
  echo "=== Task4: 正常转发 Forwarding（ryu_forward.py）==="
  local log="/tmp/task4_ryu_forward.log"
  local pid
  pid=$(start_ryu "ryu_forward.py" "${log}")
  run_mininet_block "task4" 0 1
  stop_ryu "${pid}" "ryu_forward.py"
}

run_task5() {
  echo "=== Task5: 重定向 Redirecting（ryu_redirect.py）==="
  local log="/tmp/task5_ryu_redirect.log"
  local pid
  pid=$(start_ryu "ryu_redirect.py" "${log}")
  run_mininet_block "task5" 0 1
  stop_ryu "${pid}" "ryu_redirect.py"
}

for t in "${TASKS[@]}"; do
  case "$t" in
    task1) run_task1 ;;
    task2) run_task2 ;;
    task3) run_task3 ;;
    task4) run_task4 ;;
    task5) run_task5 ;;
    *)
      usage
      exit 1
      ;;
  esac
done

echo "完成。日志: /tmp/task*_ryu_*.log /tmp/server1.log /tmp/server2.log"
