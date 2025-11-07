#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Interactive STEP client shell.
- Supports: connect, login, file save/upload, get plan, download, simple DATA KV ops.
- Progress bars via tqdm.
- Packet format: [!II]=[json_len][bin_len] + json + binary

Usage examples:
  python step_client_shell.py --server 127.0.0.1 --port 1379 --id 2025123456
  (inside shell)
    save ./big.bin my_big.bin
    get my_big.bin
    download my_big.bin ./dl.bin
    kvsave mykey {"a":1,"b":"x"}
    kvget mykey
    kvdel mykey
    exit
"""

import argparse
import cmd
import hashlib
import json
import os
import shlex
import socket
import struct
import sys
import time
from typing import Any, Dict, Optional, Tuple

from tqdm import tqdm

# ---------- Protocol constants ----------
OP_SAVE, OP_DELETE, OP_GET, OP_UPLOAD, OP_DOWNLOAD, OP_BYE, OP_LOGIN, OP_ERROR = (
    "SAVE",
    "DELETE",
    "GET",
    "UPLOAD",
    "DOWNLOAD",
    "BYE",
    "LOGIN",
    "ERROR",
)
TYPE_FILE, TYPE_DATA, TYPE_AUTH, DIR_EARTH = "FILE", "DATA", "AUTH", "EARTH"
FIELD_OPERATION = "operation"
FIELD_DIRECTION = "direction"
FIELD_TYPE = "type"
FIELD_USERNAME = "username"
FIELD_PASSWORD = "password"
FIELD_TOKEN = "token"
FIELD_KEY = "key"
FIELD_SIZE = "size"
FIELD_TOTAL_BLOCK = "total_block"
FIELD_MD5 = "md5"
FIELD_BLOCK_SIZE = "block_size"
FIELD_STATUS = "status"
FIELD_STATUS_MSG = "status_msg"
FIELD_BLOCK_INDEX = "block_index"
DIR_REQUEST, DIR_RESPONSE = "REQUEST", "RESPONSE"


def md5_file(path: str) -> str:
    h = hashlib.md5()
    with open(path, "rb") as f:
        for chunk in iter(lambda: f.read(1024 * 1024), b""):
            h.update(chunk)
    return h.hexdigest()


def pack_packet(json_data: Dict[str, Any], bin_data: Optional[bytes] = None) -> bytes:
    j = json.dumps(dict(json_data), ensure_ascii=False).encode("utf-8")
    b = bin_data or b""
    return struct.pack("!II", len(j), len(b)) + j + b


def recv_exact(sock: socket.socket, n: int) -> bytes:
    buf = b""
    while len(buf) < n:
        chunk = sock.recv(n - len(buf))
        if chunk == b"":
            raise ConnectionError("Socket closed")
        buf += chunk
    return buf


def recv_packet(sock: socket.socket) -> Tuple[Dict[str, Any], bytes]:
    hdr = recv_exact(sock, 8)
    jlen, blen = struct.unpack("!II", hdr)
    jraw = recv_exact(sock, jlen)
    try:
        j = json.loads(jraw.decode("utf-8"))
    except Exception as e:
        raise ValueError(f"Invalid JSON from server: {e}")
    b = recv_exact(sock, blen) if blen > 0 else b""
    return j, b


class StepClient:
    def __init__(self, server_ip: str, server_port: int = 1379, timeout: float = 10.0):
        self.server_ip = server_ip
        self.server_port = server_port
        self.timeout = timeout
        self.sock: Optional[socket.socket] = None
        self.token: Optional[str] = None

    def connect(self):
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(self.timeout)
        s.connect((self.server_ip, self.server_port))
        self.sock = s

    def close(self):
        try:
            if self.sock:
                self.sock.close()
        finally:
            self.sock = None

    def _send(
        self, payload: Dict[str, Any], bin_data: Optional[bytes] = None
    ) -> Tuple[Dict[str, Any], bytes]:
        assert self.sock is not None, "Not connected"
        self.sock.sendall(pack_packet(payload, bin_data))
        return recv_packet(self.sock)

    def login(self, student_id: str) -> str:
        payload = {
            FIELD_TYPE: TYPE_AUTH,
            FIELD_OPERATION: OP_LOGIN,
            FIELD_DIRECTION: DIR_REQUEST,
            FIELD_USERNAME: student_id,
            FIELD_PASSWORD: hashlib.md5(student_id.encode("utf-8")).hexdigest(),
        }
        j, _ = self._send(payload)
        if j.get(FIELD_STATUS) != 200:
            raise RuntimeError(f"LOGIN failed: {j}")
        token = j.get(FIELD_TOKEN)
        if not token:
            raise RuntimeError(f"LOGIN no token: {j}")
        self.token = token
        return token

    def _ensure_token(self):
        if not self.token:
            raise RuntimeError("No token. Call login() first.")

    # -------- DATA ops --------
    def data_get(self, key: str) -> Dict[str, Any]:
        self._ensure_token()
        payload = {
            FIELD_TYPE: TYPE_DATA,
            FIELD_OPERATION: OP_GET,
            FIELD_DIRECTION: DIR_REQUEST,
            FIELD_TOKEN: self.token,
            FIELD_KEY: key,
        }
        j, _ = self._send(payload)
        if j.get(FIELD_STATUS) != 200:
            raise RuntimeError(f"DATA GET failed: {j}")
        return j

    def data_save(self, obj: Dict[str, Any], key: Optional[str] = None) -> str:
        self._ensure_token()
        payload = {
            FIELD_TYPE: TYPE_DATA,
            FIELD_OPERATION: OP_SAVE,
            FIELD_DIRECTION: DIR_REQUEST,
            FIELD_TOKEN: self.token,
        }
        payload.update(obj)
        if key is not None:
            payload[FIELD_KEY] = key
        j, _ = self._send(payload)
        if j.get(FIELD_STATUS) != 200:
            raise RuntimeError(f"DATA SAVE failed: {j}")

        rtn = j.get(FIELD_KEY)
        if rtn is None:
            raise RuntimeError("data_save func: DATA SAVE no key returned")
        return rtn

    def data_delete(self, key: str) -> None:
        self._ensure_token()
        payload = {
            FIELD_TYPE: TYPE_DATA,
            FIELD_OPERATION: OP_DELETE,
            FIELD_DIRECTION: DIR_REQUEST,
            FIELD_TOKEN: self.token,
            FIELD_KEY: key,
        }
        j, _ = self._send(payload)
        if j.get(FIELD_STATUS) != 200:
            raise RuntimeError(f"DATA DELETE failed: {j}")

    # -------- FILE ops --------
    def file_get_plan(self, key: str) -> Dict[str, Any]:
        self._ensure_token()
        payload = {
            FIELD_TYPE: TYPE_FILE,
            FIELD_OPERATION: OP_GET,
            FIELD_DIRECTION: DIR_REQUEST,
            FIELD_TOKEN: self.token,
            FIELD_KEY: key,
        }
        j, _ = self._send(payload)
        if j.get(FIELD_STATUS) != 200:
            raise RuntimeError(f"FILE GET failed: {j}")
        return j

    def file_save_plan(
        self, file_size: int, key: Optional[str] = None
    ) -> Dict[str, Any]:
        self._ensure_token()
        payload = {
            FIELD_TYPE: TYPE_FILE,
            FIELD_OPERATION: OP_SAVE,
            FIELD_DIRECTION: DIR_REQUEST,
            FIELD_TOKEN: self.token,
            FIELD_SIZE: file_size,
        }
        if key is not None:
            payload[FIELD_KEY] = key
        j, _ = self._send(payload)
        if j.get(FIELD_STATUS) != 200:
            raise RuntimeError(f"FILE SAVE plan failed: {j}")
        return j

    def file_upload(self, path: str, key: Optional[str] = None) -> Tuple[str, str]:
        size = os.path.getsize(path)
        plan = self.file_save_plan(size, key=key)
        assigned_key = plan.get(FIELD_KEY)
        block_size = plan.get(FIELD_BLOCK_SIZE)
        total_block = plan.get(FIELD_TOTAL_BLOCK)
        if assigned_key is None or block_size is None or total_block is None:
            raise RuntimeError(f"Bad upload plan: {plan}")

        with (
            open(path, "rb") as f,
            tqdm(
                total=int(total_block),
                desc=f"UPLOAD {os.path.basename(path)}",
                unit="blk",
            ) as pbar,
        ):
            for idx in range(int(total_block)):
                f.seek(block_size * idx)
                if idx == total_block - 1:
                    chunk = f.read(size - block_size * idx)
                else:
                    chunk = f.read(block_size)
                payload = {
                    FIELD_TYPE: TYPE_FILE,
                    FIELD_OPERATION: OP_UPLOAD,
                    FIELD_DIRECTION: DIR_REQUEST,
                    FIELD_TOKEN: self.token,
                    FIELD_KEY: assigned_key,
                    FIELD_BLOCK_INDEX: idx,
                }
                j, _ = self._send(payload, chunk)
                if j.get(FIELD_STATUS) != 200:
                    raise RuntimeError(f"UPLOAD block {idx} failed: {j}")
                pbar.update(1)
                if FIELD_MD5 in j:
                    return assigned_key, j[FIELD_MD5]

        plan2 = self.file_get_plan(assigned_key)
        return assigned_key, plan2.get(FIELD_MD5, "")

    def file_download(self, key: str, out_path: str) -> str:
        plan = self.file_get_plan(key)
        size = plan[FIELD_SIZE]
        block_size = plan[FIELD_BLOCK_SIZE]
        total_block = plan[FIELD_TOTAL_BLOCK]
        server_md5 = plan[FIELD_MD5]

        with open(out_path, "wb") as f:
            f.seek(size - 1)
            f.write(b"\0")

        with (
            open(out_path, "rb+") as f,
            tqdm(total=int(total_block), desc=f"DOWNLOAD {key}", unit="blk") as pbar,
        ):
            for idx in range(int(total_block)):
                payload = {
                    FIELD_TYPE: TYPE_FILE,
                    FIELD_OPERATION: OP_DOWNLOAD,
                    FIELD_DIRECTION: DIR_REQUEST,
                    FIELD_TOKEN: self.token,
                    FIELD_KEY: key,
                    FIELD_BLOCK_INDEX: idx,
                }
                j, bin_data = self._send(payload)
                if j.get(FIELD_STATUS) != 200:
                    raise RuntimeError(f"DOWNLOAD block {idx} failed: {j}")
                f.seek(block_size * idx)
                f.write(bin_data)
                pbar.update(1)

        return server_md5


class StepShell(cmd.Cmd):
    intro = "Welcome to STEP shell. Type 'help' or '?' to list commands.\n"
    prompt = "STEP> "

    def __init__(self, server: Optional[str], port: int, student_id: Optional[str]):
        super().__init__()
        self.server = server
        self.port = port
        self.student_id = student_id
        self.client: Optional[StepClient] = None

        if self.server:
            try:
                self.do_connect(f"{self.server} {self.port}")
                if self.student_id:
                    self.do_login(self.student_id)
            except Exception as e:
                print(f"[boot] {e}")

    # ---- helpers ----
    def _need_client(self) -> StepClient:
        if not self.client:
            raise RuntimeError("Not connected. Use: connect <ip> [port]")
        return self.client

    # ---- commands ----
    def do_connect(self, arg):
        "connect <server_ip> [port] : connect to server"
        try:
            parts = shlex.split(arg)
            if not parts:
                print("Usage: connect <server_ip> [port]")
                return
            ip = parts[0]
            port = int(parts[1]) if len(parts) >= 2 else self.port
            self.client = StepClient(ip, port)
            self.client.connect()
            self.server, self.port = ip, port
            print(f"[connect] ok {ip}:{port}")
        except Exception as e:
            print(f"[connect] fail: {e}")

    def do_login(self, arg):
        "login <student_id>"
        try:
            sid = arg.strip() or (self.student_id or "")
            if not sid:
                print("Usage: login <student_id>")
                return
            cli = self._need_client()
            token = cli.login(sid)
            self.student_id = sid
            print(f"[login] token={token}")
        except Exception as e:
            print(f"[login] fail: {e}")

    def do_save(self, arg):
        "save <local_path> [key] : upload file"
        try:
            parts = shlex.split(arg)
            if not parts:
                print("Usage: save <local_path> [key]")
                return
            local = parts[0]
            key = parts[1] if len(parts) >= 2 else None
            if not os.path.isfile(local):
                print(f"[save] not a file: {local}")
                return
            cli = self._need_client()
            local_md5 = md5_file(local)
            k, remote_md5 = cli.file_upload(local, key=key)
            print(f"[save] key={k} local_md5={local_md5} server_md5={remote_md5}")
            if remote_md5 and remote_md5.lower() == local_md5.lower():
                print("[save] MD5 MATCH ✓")
            else:
                print("[save] MD5 MISMATCH ✗")
        except Exception as e:
            print(f"[save] fail: {e}")

    def do_get(self, arg):
        "get <key> : show file plan (size/block/md5)"
        try:
            key = arg.strip()
            if not key:
                print("Usage: get <key>")
                return
            cli = self._need_client()
            plan = cli.file_get_plan(key)
            print(json.dumps(plan, indent=2, ensure_ascii=False))
        except Exception as e:
            print(f"[get] fail: {e}")

    def do_download(self, arg):
        "download <key> <out_path>"
        try:
            parts = shlex.split(arg)
            if len(parts) < 2:
                print("Usage: download <key> <out_path>")
                return
            key, outp = parts[0], parts[1]
            cli = self._need_client()
            server_md5 = cli.file_download(key, outp)
            dl_md5 = md5_file(outp)
            print(f"[download] server_md5={server_md5} downloaded_md5={dl_md5}")
            if server_md5 and server_md5.lower() == dl_md5.lower():
                print("[download] MD5 MATCH ✓")
            else:
                print("[download] MD5 MISMATCH ✗")
        except Exception as e:
            print(f"[download] fail: {e}")

    def do_kvsave(self, arg):
        "kvsave <key?> <json>  (if key omitted, server assigns one)"
        try:
            parts = shlex.split(arg)
            if not parts:
                print('Usage: kvsave <key?> <json>; e.g. kvsave meta {"a":1,"b":"x"}')
                return
            if len(parts) == 1:
                key = None
                payload_s = parts[0]
            else:
                key = parts[0]
                payload_s = " ".join(parts[1:])
            obj = json.loads(payload_s)
            cli = self._need_client()
            saved_key = cli.data_save(obj, key=key)
            print(f"[kvsave] key={saved_key}")
        except Exception as e:
            print(f"[kvsave] fail: {e}")

    def do_kvget(self, arg):
        "kvget <key>"
        try:
            key = arg.strip()
            if not key:
                print("Usage: kvget <key>")
                return
            cli = self._need_client()
            j = cli.data_get(key)
            print(json.dumps(j, indent=2, ensure_ascii=False))
        except Exception as e:
            print(f"[kvget] fail: {e}")

    def do_kvdel(self, arg):
        "kvdel <key>"
        try:
            key = arg.strip()
            if not key:
                print("Usage: kvdel <key>")
                return
            cli = self._need_client()
            cli.data_delete(key)
            print("[kvdel] ok")
        except Exception as e:
            print(f"[kvdel] fail: {e}")

    def do_token(self, arg):
        "token : show current token"
        cli = self._need_client()
        print(cli.token or "<none>")

    def do_status(self, arg):
        "status : show connection status"
        print(
            json.dumps(
                {
                    "server": self.server,
                    "port": self.port,
                    "student_id": self.student_id,
                    "connected": bool(self.client and self.client.sock),
                    "has_token": bool(self.client and self.client.token),
                },
                indent=2,
            )
        )

    def do_close(self, arg):
        "close : close socket"
        if self.client:
            self.client.close()
        print("[close] ok")

    def do_exit(self, arg):
        "exit : quit shell"
        try:
            if self.client:
                self.client.close()
        finally:
            print("bye")
            return True

    def do_quit(self, arg):
        "quit : alias for exit"
        return self.do_exit(arg)

    def emptyline(self) -> bool:
        return True


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--server", help="server ip")
    ap.add_argument("--port", type=int, default=1379)
    ap.add_argument("--id", help="student id to auto-login")
    args = ap.parse_args()

    StepShell(args.server, args.port, args.id).cmdloop()


if __name__ == "__main__":
    main()
