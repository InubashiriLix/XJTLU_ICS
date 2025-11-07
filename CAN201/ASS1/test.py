#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Basic test for StepClient without interactive shell:
- starts server.py (must be in the same directory)
- creates ~10MB temp file
- login, upload, download, md5 verify
"""

import os, sys, time, socket, subprocess, tempfile
from pathlib import Path
import hashlib

SERVER_IP = "127.0.0.1"
PORT = 1379
STUDENT_ID = "2025123456"


def md5_file(path: str) -> str:
    h = hashlib.md5()
    with open(path, "rb") as f:
        for chunk in iter(lambda: f.read(1024 * 1024), b""):
            h.update(chunk)
    return h.hexdigest()


def wait_port(host, port, timeout=10.0):
    import time, socket

    deadline = time.time() + timeout
    while time.time() < deadline:
        try:
            with socket.create_connection((host, port), timeout=0.5):
                return True
        except OSError:
            time.sleep(0.2)
    return False


def main():
    # Start server
    server_py = Path("server.py")
    if not server_py.exists():
        print("server.py not found in current directory.")
        sys.exit(1)
    srv = subprocess.Popen(
        [sys.executable, str(server_py), "--ip", SERVER_IP, "--port", str(PORT)]
    )
    try:
        if not wait_port(SERVER_IP, PORT, timeout=10):
            raise RuntimeError("server not ready")

        # Import StepClient from shell module
        from client_lix import StepClient

        with tempfile.TemporaryDirectory() as td:
            src = os.path.join(td, "tenMB.bin")
            # build ~10MB file quickly
            with open(src, "wb") as f:
                f.write(os.urandom(1024))
                blk = os.urandom(4096)
                for _ in range((10 * 1024 * 1024 - 8192) // 4096):
                    f.write(blk)
                f.write(os.urandom(7168))

            cli = StepClient(SERVER_IP, PORT)
            cli.connect()
            token = cli.login(STUDENT_ID)
            print("[TEST] login ok, token len:", len(token))

            local_md5 = md5_file(src)
            key, server_md5 = cli.file_upload(src, key="tenMB.bin")
            print("[TEST] upload key:", key)
            assert local_md5.lower() == server_md5.lower(), "upload md5 mismatch"

            dst = os.path.join(td, "dl.bin")
            md5_srv = cli.file_download(key, dst)
            md5_dl = md5_file(dst)
            assert md5_srv.lower() == md5_dl.lower(), "download md5 mismatch"
            print("[TEST] all good ✓")

    finally:
        try:
            srv.terminate()
        except Exception:
            pass


if __name__ == "__main__":
    main()
