import sys
import socket
import struct
import hashlib
import time


DEFAULT_SERVER_IP = "10.0.1.2"
DEFAULT_SERVER_PORT = 5000


def build_payload() -> bytes:
    """
    Build a compact payload:
    - 8 bytes: send timestamp (seconds since epoch, float64)
    - 2 bytes: length of message (unsigned short)
    - N bytes: ASCII message
    """
    send_ts = time.time()
    message = b"CAN201_PARTII_CLIENT"
    header = struct.pack("!dH", send_ts, len(message))
    payload = header + message
    checksum = hashlib.sha256(payload).digest()
    return checksum + payload  # 32 + 8 + 2 + len(message)


def parse_args(argv):
    if len(argv) >= 3:
        server_ip = argv[1]
        try:
            server_port = int(argv[2])
        except ValueError:
            print("Invalid port, falling back to default:", DEFAULT_SERVER_PORT)
            server_port = DEFAULT_SERVER_PORT
    else:
        server_ip = DEFAULT_SERVER_IP
        server_port = DEFAULT_SERVER_PORT
    return server_ip, server_port


def run_client(server_ip: str, server_port: int):
    print(f"[client] Connecting to {server_ip}:{server_port} ...")
    start_ts = time.time()

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        sock.connect((server_ip, server_port))
        payload = build_payload()
        sock.sendall(payload)

        try:
            ack = sock.recv(1024)
            if ack:
                print("[client] Received ACK:", ack.decode(errors="ignore"))
        except socket.timeout:
            print("[client] No ACK received (timeout).")

    end_ts = time.time()
    print(f"[client] Finished in {end_ts - start_ts:.6f} seconds")
    print("[client] Capture this exchange in Wireshark/tcpdump for your report.")


def main():
    server_ip, server_port = parse_args(sys.argv)
    run_client(server_ip, server_port)


if __name__ == "__main__":
    main()
