CAN201 Networking Project – Part II
==================================

This README summarises the purpose of each Python file in the `Codes/` folder
and lists the key functions that you need to implement for the coursework.

Files in this folder
--------------------

1. `networkTopo.py`
   - Role:
     - Build the simple SDN topology required by the coursework.
     - Contains one SDN switch and three hosts: Client, Server1, Server2.
   - What you must ensure:
     - IP addresses:
       - Client  : 10.0.1.5/24
       - Server1 : 10.0.1.2/24
       - Server2 : 10.0.1.3/24
     - MAC addresses:
       - Must follow the ones given in the coursework figure.
     - Links:
       - All three hosts connect to the single SDN switch.
     - Controller:
       - The Mininet network uses a **remote** controller (Ryu) running
         on your host / WSL (IP and port configurable in `build_network()`).
   - Key functions / TODOs:
     - `SimpleSDNTopo.build()`
       - Add hosts with correct IP/MAC.
       - Add the switch.
       - Add links between hosts and switch.
     - `build_network()`
       - Create a `Mininet` instance using `SimpleSDNTopo`.
       - Configure `RemoteController` with the correct IP and port.
     - `main()`
       - Start the network (`net.start()`), perform basic tests (e.g. `pingAll()`),
         optionally open `CLI(net)`, and then stop the network.

2. `ryu_forward.py`
   - Role:
     - Ryu SDN controller application for the **forwarding case** (Task 2 + Task 4.1).
     - After the first TCP SYN from Client to Server1 triggers `Packet_In`,
       install a flow entry so that all subsequent traffic from Client to
       Server1 is forwarded to Server1.
   - Global configuration:
     - Uses OpenFlow 1.3.
     - Any non-table-miss flow entries must have `idle_timeout = 5` seconds.
   - Key functions / TODOs:
     - `add_flow(...)`
       - Already implemented as a helper to install flow entries.
       - You must call it with appropriate `priority`, `match`, and `actions`.
     - `switch_features_handler(...)`
       - Already installs a table-miss entry that sends unknown packets to the controller.
     - `build_match_for_forward(...)`   (**TO IMPLEMENT**)
       - Build a match that identifies **only** TCP packets from Client to Server1
         (and optionally the specific server port).
     - `build_actions_for_forward(...)` (**TO IMPLEMENT**)
       - Decide the output port that leads to Server1.
       - Return a list of actions that forwards the packet to Server1.
     - `packet_in_handler(...)`
       - Provided skeleton:
         - Parses Ethernet/IPv4/TCP.
         - Detects TCP SYN.
         - Calls the `build_match_*` and `build_actions_*` helpers, then installs
           the flow entry with `idle_timeout=5` and sends a `PacketOut`.
       - You may add further conditions (e.g. only handle packets with
         `ip_src == 10.0.1.5` and `ip_dst == 10.0.1.2`).

3. `ryu_redirect.py`
   - Role:
     - Ryu SDN controller application for the **redirection case** (Task 2 + Task 5.1).
     - After the first TCP SYN from Client to Server1 triggers `Packet_In`,
       install a flow so that traffic from Client to Server1 is **redirected**
       to Server2 instead.
   - Behaviour:
     - From the client's point of view, it is still talking to Server1.
     - In the network, the switch will send packets to Server2.
     - Replies from Server2 must be rewritten so that the client still sees
       them as coming from Server1.
   - Key functions / TODOs:
     - `add_flow(...)` and `switch_features_handler(...)`
       - Skeleton provided, similar to `ryu_forward.py`.
     - `build_match_for_redirect(...)`   (**TO IMPLEMENT**)
       - Match TCP flows from Client to Server1, similar to the forwarding case.
     - `build_actions_for_redirect(...)` (**TO IMPLEMENT**)
       - Use appropriate `OFPActionSetField` actions to:
         - On the forward path: change destination IP/MAC from Server1 to Server2,
           then output to Server2’s port.
         - On the reverse path: change source IP/MAC from Server2 back to Server1,
           so the client believes it is communicating with Server1.
     - `packet_in_handler(...)`
       - Very similar to `ryu_forward.py`, but calls the redirect-specific helpers.

4. `client.py`
   - Role:
     - Socket client that runs on **Client** in Mininet.
     - Drives TCP traffic towards Server1 (IP 10.0.1.2) so that your Ryu
       apps can forward/redirect the frames.
     - Used in:
       - Task 3: basic client/server testing in the SDN topology.
       - Task 4.2: packet capture and latency measurement for forwarding.
       - Task 5.2: packet capture and latency measurement for redirection.
   - Key functions / TODOs:
     - `build_payload()`
       - Decide what data the client should send (e.g. a text line, binary
         struct, or timestamp).
       - Optionally include checksums using `hashlib` or simple sequence
         numbers to help analysis.
     - `parse_args(argv)`
       - Already provided: parses `server_ip` and `server_port` from the
         command line or falls back to defaults.
     - `run_client(server_ip, server_port)`
       - Connects to the TCP server, sends the payload, and optionally reads
         a reply.
       - You may extend it to send multiple messages or to measure the
         time between `connect()` and the completion of the exchange.
     - `main()`
       - Entry point. Calls `parse_args()` and `run_client()`.

5. `server.py`
   - Role:
     - Socket server that runs on **Server1** and **Server2** in Mininet.
     - Provides a simple TCP service to support the coursework experiments.
     - Used in:
       - Task 3: used together with `client.py` to verify basic connectivity.
       - Task 4.2 and 5.2: helps you observe where the traffic actually
         terminates (Server1 vs Server2) when forwarding vs redirection
         is enabled.
   - Key functions / TODOs:
     - `handle_client(conn, addr)`
       - Read data from the connected client socket.
       - Decide how much to read and how to parse it (e.g. one `recv(4096)`,
         or a length-prefixed protocol using `struct`).
       - Optionally send a reply (e.g. echo the message or send “OK”).
     - `start_server(listen_ip, listen_port)`
       - Bind to the given IP/port (0.0.0.0:5000 by default).
       - Accept incoming TCP connections in a loop.
       - Either handle them sequentially or spawn a thread per client using
         the `threading` module (allowed by the coursework).
     - `parse_args(argv)`
       - Parse listening IP/port from command-line arguments, with sensible
         defaults for ease of testing.
     - `main()`
       - Entry point. Calls `parse_args()` and `start_server()`.

Allowed Python modules
----------------------

According to the coursework specification, **only** the following modules
are allowed in your Python files:

    os, sys, shutil, socket, struct, hashlib, math, tqdm, numpy,
    threading, multiprocessing, gzip, zlib, zipfile, time, mininet, ryu

The skeleton code in this folder only imports modules from this list
(plus the Mininet/Ryu submodules that are implied by `mininet`/`ryu`).
When you extend the code, do NOT import additional standard library
modules beyond those listed above.

High-level mapping to tasks
---------------------------

- Task 1:
  - Implement `networkTopo.py` to build the SDN topology with the correct
    IP/MAC addresses and links.

- Task 2:
  - Implement `ryu_forward.py` AND `ryu_redirect.py` so that Client,
    Server1, and Server2 can all reach each other (ping).
  - Ensure all normal flow entries use `idle_timeout = 5` seconds.

- Task 3:
  - Run `server.py` on Server1 and Server2.
  - Run `client.py` on Client.
  - Verify that the client can successfully communicate with the service
    at Server1’s IP address when the SDN controller is running.
  - Remember to wait 5 seconds after using ICMP ping, so that previous
    ICMP-induced flow entries expire before starting the TCP experiments.

- Task 4 (forwarding case):
  - Implement the forwarding logic in `ryu_forward.py`.
  - Use Wireshark/Tcpdump on Client to capture the TCP handshake packets
    when `client.py` talks to Server1.
  - Compute the networking latency from the first SYN segment to the final
    ACK in the three-way handshake, and report it in `Report_Part_II.pdf`.

- Task 5 (redirection case):
  - Implement the redirection logic in `ryu_redirect.py`.
  - Repeat the experiment with `client.py`, now with the controller
    redirecting the traffic so that Server2 handles it.
  - Capture the handshake again and measure latency.
  - Compare the results between forwarding and redirection in your report.

With this README and the skeleton Python files, you should have a clear
checklist of what to implement for both the coding part and the report.
