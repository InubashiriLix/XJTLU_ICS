#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""交互式STEP客户端Shell程序

STEP (Simple Transfer and Exchange Protocol) 客户端提供了一个交互式命令行界面，
用于与STEP服务器进行文件传输和数据操作。

主要功能：
    - 服务器连接与身份认证
    - 文件上传/下载（支持分块传输）
    - 键值数据存储操作
    - MD5校验确保数据完整性
    - 进度条显示（通过tqdm库）

数据包格式：
    [!II]=[json_len][bin_len] + json + binary
    - 前8字节：网络字节序的两个无符号整数，分别表示JSON和二进制数据的长度
    - JSON部分：包含操作元数据
    - 二进制部分：可选的二进制数据载荷

使用示例：
    命令行启动：
        python client_lix.py --server 127.0.0.1 --port 1379 --id 2025123456

    Shell内部命令：
        save ./big.bin my_big.bin      # 上传文件
        get my_big.bin                  # 获取文件信息
        download my_big.bin ./dl.bin    # 下载文件
        kvsave mykey {"a":1,"b":"x"}   # 保存键值数据
        kvget mykey                     # 获取键值数据
        kvdel mykey                     # 删除键值数据
        exit                            # 退出程序

协议类型：
    - TYPE_FILE: 文件操作
    - TYPE_DATA: 数据操作
    - TYPE_AUTH: 身份认证

操作类型：
    - OP_SAVE: 保存数据
    - OP_GET: 获取数据
    - OP_UPLOAD: 上传文件块
    - OP_DOWNLOAD: 下载文件块
    - OP_DELETE: 删除数据
    - OP_LOGIN: 登录认证
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

# ---------- 协议常量 Protocol Constants ----------
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
    """计算文件的MD5哈希值

    逐块读取文件内容并计算MD5哈希值，避免一次性加载大文件到内存。

    Args:
        path: 文件路径

    Returns:
        文件的MD5哈希值（32位十六进制字符串）

    Raises:
        FileNotFoundError: 文件不存在
        PermissionError: 无权限读取文件
    """
    h = hashlib.md5()
    with open(path, "rb") as f:
        for chunk in iter(lambda: f.read(1024 * 1024), b""):
            h.update(chunk)
    return h.hexdigest()


def pack_packet(json_data: Dict[str, Any], bin_data: Optional[bytes] = None) -> bytes:
    """打包数据为STEP协议数据包

    将JSON数据和可选的二进制数据打包成符合STEP协议格式的数据包。
    数据包格式：[json_len(4字节)][bin_len(4字节)][json数据][二进制数据]

    Args:
        json_data: 要发送的JSON数据字典
        bin_data: 可选的二进制数据，默认为None

    Returns:
        打包后的字节数据

    Example:
        >>> packet = pack_packet({"operation": "GET", "key": "test"})
        >>> packet = pack_packet({"operation": "UPLOAD"}, b"file content")
    """
    j = json.dumps(dict(json_data), ensure_ascii=False).encode("utf-8")
    b = bin_data or b""
    return struct.pack("!II", len(j), len(b)) + j + b


def recv_exact(sock: socket.socket, n: int) -> bytes:
    """从套接字精确接收指定字节数的数据

    循环接收直到获取到指定数量的字节，确保数据完整性。

    Args:
        sock: 套接字对象
        n: 需要接收的字节数

    Returns:
        接收到的字节数据

    Raises:
        ConnectionError: 连接在接收完所有数据前关闭

    Note:
        此函数会阻塞直到接收到所有请求的字节数据或连接关闭。
    """
    buf = b""
    while len(buf) < n:
        chunk = sock.recv(n - len(buf))
        if chunk == b"":
            raise ConnectionError("Socket closed")
        buf += chunk
    return buf


def recv_packet(sock: socket.socket) -> Tuple[Dict[str, Any], bytes]:
    """从套接字接收一个完整的STEP协议数据包

    按照STEP协议格式解析数据包：
    1. 读取8字节头部（json长度 + 二进制长度）
    2. 读取JSON数据并解析
    3. 读取二进制数据（如果有）

    Args:
        sock: 套接字对象

    Returns:
        元组 (json_dict, binary_data)
        - json_dict: 解析后的JSON数据字典
        - binary_data: 二进制数据，如果没有则为空字节串

    Raises:
        ConnectionError: 连接异常关闭
        ValueError: JSON数据格式无效
    """
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
    """STEP协议客户端

    提供与STEP服务器通信的完整功能，包括连接管理、身份认证、
    文件传输和键值数据操作。

    Attributes:
        server_ip: 服务器IP地址
        server_port: 服务器端口号
        timeout: 套接字超时时间（秒）
        sock: 套接字对象，连接建立后不为None
        token: 认证令牌，登录成功后获得

    Example:
        >>> client = StepClient("127.0.0.1", 1379)
        >>> client.connect()
        >>> token = client.login("2025123456")
        >>> key, md5 = client.file_upload("test.txt")
        >>> client.file_download(key, "downloaded.txt")
        >>> client.close()
    """

    def __init__(self, server_ip: str, server_port: int = 1379, timeout: float = 10.0):
        """初始化STEP客户端

        Args:
            server_ip: 服务器IP地址
            server_port: 服务器端口号，默认1379
            timeout: 套接字超时时间（秒），默认10.0秒
        """
        self.server_ip = server_ip
        self.server_port = server_port
        self.timeout = timeout
        self.sock: Optional[socket.socket] = None
        self.token: Optional[str] = None

    def connect(self):
        """连接到服务器

        创建TCP套接字并连接到指定的服务器地址。
        Raises:
            socket.error: 连接失败
            socket.timeout: 连接超时
        """
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(self.timeout)
        s.connect((self.server_ip, self.server_port))
        self.sock = s

    def close(self):
        """关闭与服务器的连接

        安全地关闭套接字连接并清理资源。
        """
        try:
            if self.sock:
                self.sock.close()
        finally:
            self.sock = None

    def _send(
        self, payload: Dict[str, Any], bin_data: Optional[bytes] = None
    ) -> Tuple[Dict[str, Any], bytes]:
        """发送数据包并接收响应（内部方法）

        Args:
            payload: JSON数据载荷
            bin_data: 可选的二进制数据

        Returns:
            元组 (响应JSON, 响应二进制数据)

        Raises:
            AssertionError: 未连接到服务器
            ConnectionError: 连接异常
        """
        assert self.sock is not None, "Not connected"
        self.sock.sendall(pack_packet(payload, bin_data))
        return recv_packet(self.sock)

    def login(self, student_id: str) -> str:
        """用户登录认证

        使用学号进行身份认证。密码为学号的MD5哈希值。

        Args:
            student_id: 学生ID

        Returns:
            认证令牌字符串

        Raises:
            RuntimeError: 登录失败或服务器未返回令牌

        Note:
            认证成功后，令牌会自动保存到self.token，用于后续操作。
        """
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
        """确保已获取认证令牌（内部方法）

        Raises:
            RuntimeError: 未登录（没有令牌）
        """
        if not self.token:
            raise RuntimeError("No token. Call login() first.")

    # -------- DATA操作 Data Operations --------
    def data_get(self, key: str) -> Dict[str, Any]:
        """获取键值数据

        从服务器获取指定键的数据。

        Args:
            key: 数据键

        Returns:
            包含数据内容的字典，格式由服务器定义

        Raises:
            RuntimeError: 未登录或获取失败
        """
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
        """保存键值数据

        将数据对象保存到服务器。如果不指定键，服务器会自动分配一个。

        Args:
            obj: 要保存的数据字典
            key: 可选的数据键，如果为None则由服务器分配

        Returns:
            保存后的数据键（可能是服务器分配的新键）

        Raises:
            RuntimeError: 未登录、保存失败或服务器未返回键

        Example:
            >>> key = client.data_save({"name": "test", "value": 123})
            >>> key = client.data_save({"data": "test"}, key="mydata")
        """
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
        """删除键值数据

        从服务器删除指定键的数据。

        Args:
            key: 要删除的数据键

        Raises:
            RuntimeError: 未登录或删除失败
        """
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

    # -------- FILE操作 File Operations --------
    def file_get_plan(self, key: str) -> Dict[str, Any]:
        """获取文件传输计划

        从服务器获取文件的元数据，包括大小、分块信息、MD5等。

        Args:
            key: 文件键

        Returns:
            包含文件信息的字典：
            - size: 文件总大小（字节）
            - block_size: 每个块的大小（字节）
            - total_block: 总块数
            - md5: 文件MD5哈希值

        Raises:
            RuntimeError: 未登录或获取失败
        """
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
        """创建文件保存计划

        向服务器请求文件上传计划，获取分块信息和文件键。

        Args:
            file_size: 文件大小（字节）
            key: 可选的文件键，如果为None则由服务器分配

        Returns:
            服务器返回的上传计划字典：
            - key: 分配的文件键
            - block_size: 块大小
            - total_block: 总块数

        Raises:
            RuntimeError: 未登录或请求失败
        """
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
        """上传文件到服务器

        将本地文件分块上传到服务器，并显示进度条。

        Args:
            path: 本地文件路径
            key: 可选的文件键，如果为None则由服务器分配

        Returns:
            元组 (文件键, 服务器MD5)
            - 文件键: 文件在服务器上的唯一标识
            - 服务器MD5: 服务器计算的文件MD5哈希值

        Raises:
            RuntimeError: 未登录、上传失败或计划信息不完整
            FileNotFoundError: 文件不存在

        Note:
            上传过程中会显示tqdm进度条。
            建议上传后比对本地和服务器的MD5值以确保完整性。
        """
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
        """从服务器下载文件

        从服务器分块下载文件到本地，并显示进度条。

        Args:
            key: 服务器上的文件键
            out_path: 本地输出文件路径

        Returns:
            服务器返回的文件MD5哈希值

        Raises:
            RuntimeError: 未登录或下载失败

        Note:
            下载过程中会显示tqdm进度条。
            文件会先预分配空间，然后按块写入，确保大文件下载的稳定性。
            建议下载后比对本地和服务器的MD5值以确保完整性。
        """
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
    """STEP交互式Shell

    提供命令行界面用于与STEP服务器交互。支持文件上传下载、
    键值数据操作等功能。

    Attributes:
        intro: Shell启动时的欢迎信息
        prompt: 命令提示符
        server: 服务器IP地址
        port: 服务器端口
        student_id: 学生ID
        client: StepClient实例

    支持的命令：
        connect: 连接到服务器
        login: 用户登录
        save: 上传文件
        get: 获取文件信息
        download: 下载文件
        kvsave: 保存键值数据
        kvget: 获取键值数据
        kvdel: 删除键值数据
        token: 显示当前令牌
        status: 显示连接状态
        close: 关闭连接
        exit/quit: 退出Shell
    """

    intro = "Welcome to STEP shell. Type 'help' or '?' to list commands.\n"
    prompt = "STEP> "

    def __init__(self, server: Optional[str], port: int, student_id: Optional[str]):
        """初始化Shell

        Args:
            server: 服务器IP地址，如果提供则自动连接
            port: 服务器端口
            student_id: 学生ID，如果提供则自动登录
        """
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

    # ---- 辅助方法 Helper Methods ----
    def _need_client(self) -> StepClient:
        """确保客户端已连接（内部方法）

        Returns:
            StepClient实例

        Raises:
            RuntimeError: 未连接到服务器
        """
        if not self.client:
            raise RuntimeError("Not connected. Use: connect <ip> [port]")
        return self.client

    # ---- Shell命令 Shell Commands ----
    def do_connect(self, arg):
        """连接到服务器

        用法: connect <server_ip> [port]

        Args:
            arg: 参数字符串，包含服务器IP和可选的端口号
        """
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
        """用户登录

        用法: login <student_id>

        Args:
            arg: 学生ID
        """
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
        """上传文件到服务器

        用法: save <local_path> [key]

        Args:
            arg: 参数字符串，包含本地文件路径和可选的文件键

        Note:
            上传成功后会显示MD5校验结果。
        """
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
        """获取文件信息

        用法: get <key>

        显示文件的传输计划，包括大小、分块信息和MD5。

        Args:
            arg: 文件键
        """
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
        """从服务器下载文件

        用法: download <key> <out_path>

        Args:
            arg: 参数字符串，包含文件键和输出路径

        Note:
            下载成功后会显示MD5校验结果。
        """
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
        """保存键值数据

        用法: kvsave <key> <json> 或 kvsave <json>

        Args:
            arg: 参数字符串，包含可选的键和JSON数据

        Example:
            kvsave meta {"a":1,"b":"x"}     # 使用指定键
            kvsave {"name":"test"}          # 服务器自动分配键
        """
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
        """获取键值数据

        用法: kvget <key>

        Args:
            arg: 数据键
        """
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
        """删除键值数据

        用法: kvdel <key>

        Args:
            arg: 要删除的数据键
        """
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
        """显示当前认证令牌

        用法: token
        """
        "token : show current token"
        cli = self._need_client()
        print(cli.token or "<none>")

    def do_status(self, arg):
        """显示连接状态

        用法: status

        显示服务器地址、端口、学生ID、连接状态和令牌状态。
        """
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
        """关闭套接字连接

        用法: close
        """
        "close : close socket"
        if self.client:
            self.client.close()
        print("[close] ok")

    def do_exit(self, arg):
        """退出Shell

        用法: exit

        退出前会自动关闭连接。
        """
        "exit : quit shell"
        try:
            if self.client:
                self.client.close()
        finally:
            print("bye")
            return True

    def do_quit(self, arg):
        """退出Shell（exit的别名）

        用法: quit
        """
        "quit : alias for exit"
        return self.do_exit(arg)

    def emptyline(self) -> bool:
        """处理空行输入

        Returns:
            True表示不执行任何操作（覆盖默认的重复上一条命令行为）
        """
        return True


def main():
    """主函数

    解析命令行参数并启动STEP Shell。

    命令行参数：
        --server: 服务器IP地址
        --port: 服务器端口（默认1379）
        --id: 学生ID（用于自动登录）

    Example:
        python client_lix.py --server 127.0.0.1 --port 1379 --id 2025123456
    """
    ap = argparse.ArgumentParser()
    ap.add_argument("--server", help="server ip")
    ap.add_argument("--port", type=int, default=1379)
    ap.add_argument("--id", help="student id to auto-login")
    args = ap.parse_args()

    StepShell(args.server, args.port, args.id).cmdloop()


if __name__ == "__main__":
    main()
