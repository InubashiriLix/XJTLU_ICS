import sys
import socket
import struct
import hashlib
import time
import threading


DEFAULT_LISTEN_IP = "0.0.0.0"
DEFAULT_LISTEN_PORT = 5000


def recv_exact(conn: socket.socket, size: int) -> bytes:
    chunks = []
    remaining = size
    while remaining > 0:
        data = conn.recv(remaining)
        if not data:
            break
        chunks.append(data)
        remaining -= len(data)
    return b"".join(chunks)


def handle_client(conn: socket.socket, addr):
    print(f"[server] Accepted connection from {addr}")

    try:
        header = recv_exact(conn, 32 + 8 + 2)  # checksum + timestamp + length
        if len(header) < 42:
            print("[server] Incomplete header, closing.")
            return

        recv_checksum = header[:32]
        ts_bytes = header[32:40]
        msglen_bytes = header[40:42]

        try:
            send_ts = struct.unpack("!d", ts_bytes)[0]
            msg_len = struct.unpack("!H", msglen_bytes)[0]
        except struct.error:
            print("[server] Failed to unpack header.")
            return

        message = recv_exact(conn, msg_len)
        full_payload = ts_bytes + msglen_bytes + message
        calc_checksum = hashlib.sha256(full_payload).digest()

        print(
            f"[server] Payload len={msg_len}, valid_checksum={calc_checksum == recv_checksum}"
        )
        print(f"[server] Client->Server app latency: {(time.time() - send_ts):.6f} s")

        ack = f"ACK len={msg_len} ts={send_ts:.6f}".encode("ascii")
        conn.sendall(ack)
    finally:
        conn.close()
        print(f"[server] Connection with {addr} closed")


def start_server(listen_ip: str, listen_port: int):
    print(f"[server] Starting TCP server on {listen_ip}:{listen_port} ...")

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as srv:
        srv.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        srv.bind((listen_ip, listen_port))
        srv.listen()
        print("[server] Listening for incoming connections ...")

        while True:
            conn, addr = srv.accept()
            # Lightweight, so sequential handling is fine; flip to threaded if needed.
            handle_client(conn, addr)
            # threading.Thread(target=handle_client, args=(conn, addr), daemon=True).start()


def parse_args(argv):
    if len(argv) >= 3:
        listen_ip = argv[1]
        try:
            listen_port = int(argv[2])
        except ValueError:
            print("Invalid port, falling back to default:", DEFAULT_LISTEN_PORT)
            listen_port = DEFAULT_LISTEN_PORT
    else:
        listen_ip = DEFAULT_LISTEN_IP
        listen_port = DEFAULT_LISTEN_PORT
    return listen_ip, listen_port


def main():
    listen_ip, listen_port = parse_args(sys.argv)
    start_server(listen_ip, listen_port)


if __name__ == "__main__":
    main()
