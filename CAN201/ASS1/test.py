#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试流程：
    1. 自动启动server.py服务器进程
    2. 创建约10MB的临时测试文件
    3. 执行登录认证
    4. 上传测试文件并验证MD5
    5. 下载文件并验证MD5
    6. 清理临时文件并关闭服务器

测试内容：
    - 服务器启动和端口监听
    - 客户端连接和登录
    - 大文件分块上传
    - 文件下载功能
    - MD5完整性校验

使用方法：
    python test.py

注意事项：
    - server.py必须与test.py在同一目录
    - 确保端口1379未被占用
    - 测试完成后会自动清理临时文件和关闭服务器

常量配置：
    SERVER_IP: 服务器地址（127.0.0.1）
    PORT: 服务器端口（1379）
    STUDENT_ID: 测试用学生ID（2025123456）
"""

import os, sys, time, socket, subprocess, tempfile
from pathlib import Path
import hashlib

# 测试配置常量 Test Configuration Constants
SERVER_IP = "127.0.0.1"
PORT = 1379
STUDENT_ID = "2025123456"


def md5_file(path: str) -> str:
    """计算文件的MD5哈希值

    逐块读取文件内容并计算MD5哈希值，避免一次性加载大文件到内存。
    每次读取1MB数据块进行处理。

    Args:
        path: 文件路径

    Returns:
        文件的MD5哈希值（32位十六进制字符串）

    Raises:
        FileNotFoundError: 文件不存在
        PermissionError: 无权限读取文件

    Example:
        >>> md5 = md5_file("test.bin")
        >>> print(md5)
        'd41d8cd98f00b204e9800998ecf8427e'
    """
    h = hashlib.md5()
    with open(path, "rb") as f:
        for chunk in iter(lambda: f.read(1024 * 1024), b""):
            h.update(chunk)
    return h.hexdigest()


def wait_port(host, port, timeout=10.0):
    """等待指定端口变为可连接状态

    循环尝试连接指定的主机和端口，直到连接成功或超时。
    用于等待服务器进程启动完成。

    Args:
        host: 主机地址（IP或域名）
        port: 端口号
        timeout: 超时时间（秒），默认10.0秒

    Returns:
        bool: 如果端口在超时前变为可连接返回True，否则返回False

    Note:
        每次尝试连接的超时为0.5秒，失败后等待0.2秒再重试。

    Example:
        >>> if wait_port("127.0.0.1", 1379, timeout=5):
        ...     print("Server is ready")
        ... else:
        ...     print("Server failed to start")
    """
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
    """主测试函数

    执行完整的STEP客户端集成测试流程：

    测试步骤：
        1. 检查server.py是否存在
        2. 启动服务器进程
        3. 等待服务器端口就绪
        4. 创建临时目录和测试文件（约10MB）
        5. 创建StepClient并连接服务器
        6. 执行登录认证
        7. 上传测试文件并验证MD5
        8. 下载文件并验证MD5
        9. 清理资源并终止服务器

    测试文件生成策略：
        - 前1KB：随机数据
        - 中间部分：重复相同的4KB块（提高生成速度）
        - 最后7KB：随机数据
        - 总大小：约10MB

    Raises:
        SystemExit: server.py不存在时退出
        RuntimeError: 服务器未能在超时时间内启动
        AssertionError: MD5校验失败

    Note:
        测试使用临时目录存储测试文件，测试完成后自动清理。
        无论测试成功或失败，都会尝试终止服务器进程。
    """
    # 检查服务器脚本是否存在
    server_py = Path("server.py")
    if not server_py.exists():
        print("server.py not found in current directory.")
        sys.exit(1)

    # 启动服务器进程
    srv = subprocess.Popen(
        [sys.executable, str(server_py), "--ip", SERVER_IP, "--port", str(PORT)]
    )
    try:
        # 等待服务器端口就绪
        if not wait_port(SERVER_IP, PORT, timeout=10):
            raise RuntimeError("server not ready")

        # 从client_lix模块导入StepClient
        from client_lix import StepClient

        # 使用临时目录进行测试
        with tempfile.TemporaryDirectory() as td:
            src = os.path.join(td, "tenMB.bin")

            # 快速生成约10MB的测试文件
            # 策略：使用重复的数据块以提高生成速度
            with open(src, "wb") as f:
                f.write(os.urandom(1024))  # 前1KB随机数据
                blk = os.urandom(4096)  # 生成一个4KB的随机块
                # 写入中间部分（重复相同的4KB块）
                for _ in range((10 * 1024 * 1024 - 8192) // 4096):
                    f.write(blk)
                f.write(os.urandom(7168))  # 最后7KB随机数据

            # 创建客户端并连接
            cli = StepClient(SERVER_IP, PORT)
            cli.connect()

            # 登录认证
            token = cli.login(STUDENT_ID)
            print("[TEST] login ok, token len:", len(token))

            # 测试文件上传
            local_md5 = md5_file(src)
            key, server_md5 = cli.file_upload(src, key="tenMB.bin")
            print("[TEST] upload key:", key)
            assert local_md5.lower() == server_md5.lower(), "upload md5 mismatch"

            # 测试文件下载
            dst = os.path.join(td, "dl.bin")
            md5_srv = cli.file_download(key, dst)
            md5_dl = md5_file(dst)
            assert md5_srv.lower() == md5_dl.lower(), "download md5 mismatch"
            print("[TEST] all good ✓")

    finally:
        # 清理：终止服务器进程
        try:
            srv.terminate()
        except Exception:
            pass


if __name__ == "__main__":
    main()
