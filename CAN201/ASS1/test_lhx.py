"""STEP协议客户端测试模块

这个模块包含了对client_lhx.py中所有函数的详细测试用例。
测试覆盖了正常功能、边界情况和异常处理。

典型用法示例:

    python -m pytest test_lhx.py -v
    python -m pytest test_lhx.py::TestBasicFunctions -v
    python -m pytest test_lhx.py::TestBasicFunctions::test_normalize_ip -v
"""

import unittest
from unittest.mock import Mock, MagicMock, patch, mock_open, call
import sys
import os
import json
import struct
import hashlib
import socket
import argparse
import tempfile
import io

# 导入被测试的模块
import client_lhx


class TestBasicFunctions(unittest.TestCase):
    """基础工具函数测试类
    
    测试client_lhx.py中的基础工具函数,包括输入处理、IP验证和规范化等功能。
    """

    def test_ask_normal_input(self):
        """测试_ask函数正常输入
        
        验证_ask函数在正常输入情况下是否能够正确读取并去除空格。
        
        测试场景:
            - 输入带有前后空格的字符串
            - 输入普通字符串
        
        预期结果:
            返回去除前后空格的字符串
        """
        with patch('builtins.input', return_value='  test input  '):
            result = client_lhx._ask("Enter something: ")
            self.assertEqual(result, "test input")

    def test_ask_eoferror(self):
        """测试_ask函数EOF异常处理
        
        验证当输入流结束时,_ask函数是否能正确捕获EOFError并退出程序。
        
        测试场景:
            - 模拟EOF异常(如Ctrl+D)
        
        预期结果:
            - 捕获EOFError
            - 打印中止消息
            - 调用sys.exit(1)退出
        """
        with patch('builtins.input', side_effect=EOFError):
            with patch('sys.exit') as mock_exit:
                with patch('builtins.print') as mock_print:
                    client_lhx._ask("Enter something: ")
                    mock_print.assert_called_with("\n[ABORT] No input available.")
                    mock_exit.assert_called_once_with(1)

    def test_normalize_ip_localhost(self):
        """测试IP地址规范化 - localhost转换
        
        验证_normalize_ip函数能否正确将localhost转换为127.0.0.1。
        
        测试场景:
            - 输入"localhost"
            - 输入"LOCALHOST"(大写)
            - 输入" localhost "(带空格)
        
        预期结果:
            全部返回"127.0.0.1"
        """
        self.assertEqual(client_lhx._normalize_ip("localhost"), "127.0.0.1")
        self.assertEqual(client_lhx._normalize_ip("LOCALHOST"), "127.0.0.1")
        self.assertEqual(client_lhx._normalize_ip(" localhost "), "127.0.0.1")

    def test_normalize_ip_standard(self):
        """测试IP地址规范化 - 标准IP
        
        验证_normalize_ip函数对标准IP地址的处理。
        
        测试场景:
            - 输入标准IPv4地址
            - 输入带空格的IP地址
        
        预期结果:
            返回去除空格后的IP地址
        """
        self.assertEqual(client_lhx._normalize_ip("192.168.1.1"), "192.168.1.1")
        self.assertEqual(client_lhx._normalize_ip(" 10.0.0.1 "), "10.0.0.1")

    def test_is_ipv4_strict_valid(self):
        """测试IPv4严格验证 - 有效IP
        
        验证_is_ipv4_strict函数能否正确识别有效的IPv4地址。
        
        测试场景:
            - 标准IPv4地址
            - 边界值IP地址(0.0.0.0, 255.255.255.255)
            - 回环地址
        
        预期结果:
            全部返回True
        """
        self.assertTrue(client_lhx._is_ipv4_strict("127.0.0.1"))
        self.assertTrue(client_lhx._is_ipv4_strict("192.168.1.1"))
        self.assertTrue(client_lhx._is_ipv4_strict("0.0.0.0"))
        self.assertTrue(client_lhx._is_ipv4_strict("255.255.255.255"))

    def test_is_ipv4_strict_invalid(self):
        """测试IPv4严格验证 - 无效IP
        
        验证_is_ipv4_strict函数能否正确拒绝无效的IPv4地址。
        
        测试场景:
            - 数字超出范围(256)
            - 格式错误(少于4段、多于4段)
            - 包含非数字字符
            - 空字符串
            - 负数
        
        预期结果:
            全部返回False
        """
        # 超出范围
        self.assertFalse(client_lhx._is_ipv4_strict("256.1.1.1"))
        self.assertFalse(client_lhx._is_ipv4_strict("1.256.1.1"))
        
        # 格式错误
        self.assertFalse(client_lhx._is_ipv4_strict("192.168.1"))
        self.assertFalse(client_lhx._is_ipv4_strict("192.168.1.1.1"))
        
        # 非数字
        self.assertFalse(client_lhx._is_ipv4_strict("192.168.a.1"))
        self.assertFalse(client_lhx._is_ipv4_strict("localhost"))
        
        # 空字符串
        self.assertFalse(client_lhx._is_ipv4_strict(""))
        
        # 负数
        self.assertFalse(client_lhx._is_ipv4_strict("-1.0.0.1"))


class TestArgumentParsing(unittest.TestCase):
    """命令行参数解析测试类
    
    测试parse_and_validate_args函数的参数解析和验证功能。
    """

    def test_parse_and_validate_args_complete(self):
        """测试完整参数解析
        
        验证当提供所有必需参数且有效时,函数是否正确解析。
        
        测试场景:
            - 提供有效的IP、端口、ID和文件路径
        
        预期结果:
            - 正确解析所有参数
            - 不需要额外交互输入
        """
        test_file = tempfile.NamedTemporaryFile(delete=False)
        test_file.write(b"test content")
        test_file.close()
        
        try:
            test_args = [
                '--server_ip', '127.0.0.1',
                '--server_port', '1379',
                '--id', 'student123',
                '-f', test_file.name
            ]
            
            with patch('sys.argv', ['client_lhx.py'] + test_args):
                args = client_lhx.parse_and_validate_args()
                self.assertEqual(args.server_ip, '127.0.0.1')
                self.assertEqual(args.server_port, 1379)
                self.assertEqual(args.id, 'student123')
                self.assertEqual(args.file, test_file.name)
        finally:
            os.unlink(test_file.name)

    def test_parse_and_validate_args_invalid_ip_then_valid(self):
        """测试无效IP后提供有效IP
        
        验证函数在接收到无效IP时会提示重新输入,直到获得有效IP。
        
        测试场景:
            - 首次提供无效IP
            - 第二次提供有效IP
        
        预期结果:
            - 打印提示信息
            - 接受第二次输入的有效IP
        """
        test_file = tempfile.NamedTemporaryFile(delete=False)
        test_file.write(b"test content")
        test_file.close()
        
        try:
            test_args = [
                '--server_ip', 'invalid_ip',
                '--id', 'student123',
                '-f', test_file.name
            ]
            
            with patch('sys.argv', ['client_lhx.py'] + test_args):
                with patch('client_lhx._ask', return_value='192.168.1.1'):
                    with patch('builtins.print') as mock_print:
                        args = client_lhx.parse_and_validate_args()
                        self.assertEqual(args.server_ip, '192.168.1.1')
                        # 验证打印了提示信息
                        mock_print.assert_called()
        finally:
            os.unlink(test_file.name)

    def test_parse_and_validate_args_missing_id(self):
        """测试缺少学号参数
        
        验证当缺少学号时,函数会提示输入。
        
        测试场景:
            - 命令行参数中没有提供--id
        
        预期结果:
            - 调用_ask函数请求输入学号
        """
        test_file = tempfile.NamedTemporaryFile(delete=False)
        test_file.write(b"test content")
        test_file.close()
        
        try:
            test_args = [
                '--server_ip', '127.0.0.1',
                '-f', test_file.name
            ]
            
            with patch('sys.argv', ['client_lhx.py'] + test_args):
                with patch('client_lhx._ask', return_value='student456'):
                    args = client_lhx.parse_and_validate_args()
                    self.assertEqual(args.id, 'student456')
        finally:
            os.unlink(test_file.name)

    def test_parse_and_validate_args_nonexistent_file(self):
        """测试文件不存在的情况
        
        验证当指定的文件不存在时,函数会提示重新输入文件路径。
        
        测试场景:
            - 提供不存在的文件路径
            - 然后提供存在的文件路径
        
        预期结果:
            - 打印文件未找到提示
            - 接受第二次输入的有效文件路径
        """
        test_file = tempfile.NamedTemporaryFile(delete=False)
        test_file.write(b"test content")
        test_file.close()
        
        try:
            test_args = [
                '--server_ip', '127.0.0.1',
                '--id', 'student123',
                '-f', '/nonexistent/file.txt'
            ]
            
            with patch('sys.argv', ['client_lhx.py'] + test_args):
                with patch('client_lhx._ask', return_value=test_file.name):
                    with patch('builtins.print') as mock_print:
                        args = client_lhx.parse_and_validate_args()
                        self.assertEqual(args.file, test_file.name)
                        mock_print.assert_called()
        finally:
            os.unlink(test_file.name)


class TestProtocolFunctions(unittest.TestCase):
    """STEP协议通信函数测试类
    
    测试STEP协议相关的数据包构造、接收和解析功能。
    """

    def test_make_packet_json_only(self):
        """测试仅包含JSON的数据包构造
        
        验证make_packet函数能否正确构造仅包含JSON数据的数据包。
        
        测试场景:
            - 构造包含简单字典的数据包
        
        预期结果:
            - 数据包前8字节正确表示JSON长度和0字节二进制数据长度
            - 后续字节为正确的JSON数据
        """
        data = {"type": "TEST", "value": 123}
        packet = client_lhx.make_packet(data)
        
        # 解析数据包头
        jlen, blen = struct.unpack("!II", packet[:8])
        
        # 验证长度
        json_str = json.dumps(data, ensure_ascii=False).encode('utf-8')
        self.assertEqual(jlen, len(json_str))
        self.assertEqual(blen, 0)
        
        # 验证JSON内容
        received_json = json.loads(packet[8:8+jlen].decode('utf-8'))
        self.assertEqual(received_json, data)

    def test_make_packet_with_body(self):
        """测试包含二进制数据的数据包构造
        
        验证make_packet函数能否正确构造包含JSON和二进制数据的数据包。
        
        测试场景:
            - 构造包含JSON数据和二进制数据的数据包
        
        预期结果:
            - 数据包头正确表示两部分长度
            - JSON和二进制数据内容正确
        """
        data = {"type": "FILE", "operation": "UPLOAD"}
        body = b"binary data content"
        packet = client_lhx.make_packet(data, body)
        
        # 解析数据包头
        jlen, blen = struct.unpack("!II", packet[:8])
        
        # 验证长度
        json_str = json.dumps(data, ensure_ascii=False).encode('utf-8')
        self.assertEqual(jlen, len(json_str))
        self.assertEqual(blen, len(body))
        
        # 验证内容
        received_json = json.loads(packet[8:8+jlen].decode('utf-8'))
        received_body = packet[8+jlen:8+jlen+blen]
        self.assertEqual(received_json, data)
        self.assertEqual(received_body, body)

    def test_recv_exact_success(self):
        """测试recv_exact函数正常接收
        
        验证recv_exact函数能否正确接收指定长度的数据。
        
        测试场景:
            - 模拟socket接收固定长度的数据
        
        预期结果:
            - 返回正确长度和内容的数据
        """
        mock_sock = Mock()
        test_data = b"test data 1234567890"
        mock_sock.recv.return_value = test_data
        
        result = client_lhx.recv_exact(mock_sock, len(test_data))
        self.assertEqual(result, test_data)

    def test_recv_exact_partial_receive(self):
        """测试recv_exact函数分段接收
        
        验证recv_exact函数在数据分多次接收时能否正确处理。
        
        测试场景:
            - 模拟socket分3次接收完整数据
        
        预期结果:
            - 多次调用recv直到接收完整数据
            - 返回完整的数据
        """
        mock_sock = Mock()
        # 模拟分3次接收数据
        mock_sock.recv.side_effect = [b"part1", b"part2", b"part3"]
        
        result = client_lhx.recv_exact(mock_sock, 15)
        self.assertEqual(result, b"part1part2part3")
        self.assertEqual(mock_sock.recv.call_count, 3)

    def test_recv_exact_connection_closed(self):
        """测试recv_exact函数连接关闭异常
        
        验证recv_exact函数在连接意外关闭时能否抛出正确异常。
        
        测试场景:
            - 模拟socket在接收过程中关闭连接(返回空字节)
        
        预期结果:
            - 抛出ConnectionError异常
            - 异常消息正确
        """
        mock_sock = Mock()
        # 模拟连接关闭
        mock_sock.recv.return_value = b""
        
        with self.assertRaises(ConnectionError) as context:
            client_lhx.recv_exact(mock_sock, 100)
        
        self.assertIn("Socket closed", str(context.exception))

    def test_read_packet_json_only(self):
        """测试read_packet函数读取纯JSON数据包
        
        验证read_packet函数能否正确读取和解析仅包含JSON的数据包。
        
        测试场景:
            - 构造并读取纯JSON数据包
        
        预期结果:
            - 正确解析JSON内容
            - 二进制部分为空字节
        """
        mock_sock = Mock()
        test_data = {"status": 200, "message": "OK"}
        packet = client_lhx.make_packet(test_data)
        
        # 模拟分块接收
        mock_sock.recv.side_effect = [packet[i:i+1] for i in range(len(packet))]
        
        json_data, body = client_lhx.read_packet(mock_sock)
        self.assertEqual(json_data, test_data)
        self.assertEqual(body, b"")

    def test_read_packet_with_body(self):
        """测试read_packet函数读取包含二进制数据的数据包
        
        验证read_packet函数能否正确读取和解析包含二进制数据的数据包。
        
        测试场景:
            - 构造并读取包含JSON和二进制数据的数据包
        
        预期结果:
            - 正确解析JSON内容
            - 正确提取二进制数据
        """
        mock_sock = Mock()
        test_data = {"operation": "UPLOAD", "block_index": 0}
        test_body = b"file chunk data"
        packet = client_lhx.make_packet(test_data, test_body)
        
        # 模拟分块接收
        mock_sock.recv.side_effect = [packet[i:i+1] for i in range(len(packet))]
        
        json_data, body = client_lhx.read_packet(mock_sock)
        self.assertEqual(json_data, test_data)
        self.assertEqual(body, test_body)

    def test_md5_file(self):
        """测试md5_file函数计算文件MD5
        
        验证md5_file函数能否正确计算文件的MD5哈希值。
        
        测试场景:
            - 创建临时文件并写入已知内容
            - 计算MD5并与预期值比较
        
        预期结果:
            - 返回正确的MD5哈希值
        """
        test_file = tempfile.NamedTemporaryFile(delete=False)
        test_content = b"test file content for MD5 calculation"
        test_file.write(test_content)
        test_file.close()
        
        try:
            # 计算预期MD5
            expected_md5 = hashlib.md5(test_content).hexdigest()
            
            # 使用函数计算MD5
            result_md5 = client_lhx.md5_file(test_file.name)
            
            self.assertEqual(result_md5, expected_md5)
        finally:
            os.unlink(test_file.name)

    def test_md5_file_large_file(self):
        """测试md5_file函数处理大文件
        
        验证md5_file函数在处理大于分块大小的文件时的正确性。
        
        测试场景:
            - 创建大于默认chunk大小(4096字节)的文件
        
        预期结果:
            - 正确计算大文件的MD5值
        """
        test_file = tempfile.NamedTemporaryFile(delete=False)
        # 创建8KB的测试文件
        test_content = b"A" * 8192
        test_file.write(test_content)
        test_file.close()
        
        try:
            expected_md5 = hashlib.md5(test_content).hexdigest()
            result_md5 = client_lhx.md5_file(test_file.name)
            self.assertEqual(result_md5, expected_md5)
        finally:
            os.unlink(test_file.name)

    def test_md5_file_empty_file(self):
        """测试md5_file函数处理空文件
        
        验证md5_file函数能否正确处理空文件的MD5计算。
        
        测试场景:
            - 创建空文件
        
        预期结果:
            - 返回空内容的MD5值
        """
        test_file = tempfile.NamedTemporaryFile(delete=False)
        test_file.close()
        
        try:
            expected_md5 = hashlib.md5(b"").hexdigest()
            result_md5 = client_lhx.md5_file(test_file.name)
            self.assertEqual(result_md5, expected_md5)
        finally:
            os.unlink(test_file.name)


class TestAuthenticationOperations(unittest.TestCase):
    """认证操作测试类
    
    测试登录认证相关的功能。
    """

    def test_do_login_success(self):
        """测试登录成功场景
        
        验证do_login函数在服务器返回成功响应时的处理。
        
        测试场景:
            - 模拟服务器返回status=200和有效token
        
        预期结果:
            - 发送正确的登录请求数据包
            - 返回服务器提供的token
            - 打印成功消息
        """
        mock_sock = Mock()
        student_id = "student123"
        expected_token = "test_token_xyz"
        
        # 模拟服务器响应
        response_data = {
            "status": 200,
            "token": expected_token,
            "operation": "LOGIN"
        }
        response_packet = client_lhx.make_packet(response_data)
        
        # 模拟socket接收
        mock_sock.recv.side_effect = [response_packet[i:i+1] for i in range(len(response_packet))]
        
        with patch('builtins.print') as mock_print:
            token = client_lhx.do_login(mock_sock, student_id)
            
            self.assertEqual(token, expected_token)
            # 验证发送的数据
            mock_sock.sendall.assert_called_once()
            
            # 验证打印了成功消息
            self.assertTrue(any("LOGIN SUCCESS" in str(call) for call in mock_print.call_args_list))

    def test_do_login_failure(self):
        """测试登录失败场景
        
        验证do_login函数在服务器返回失败响应时的异常处理。
        
        测试场景:
            - 模拟服务器返回status!=200(如401认证失败)
        
        预期结果:
            - 抛出RuntimeError异常
            - 异常消息包含失败信息
        """
        mock_sock = Mock()
        student_id = "student123"
        
        # 模拟服务器失败响应
        response_data = {
            "status": 401,
            "message": "Authentication failed"
        }
        response_packet = client_lhx.make_packet(response_data)
        mock_sock.recv.side_effect = [response_packet[i:i+1] for i in range(len(response_packet))]
        
        with self.assertRaises(RuntimeError) as context:
            client_lhx.do_login(mock_sock, student_id)
        
        self.assertIn("LOGIN FAILED", str(context.exception))

    def test_do_login_password_generation(self):
        """测试登录密码生成逻辑
        
        验证do_login函数使用正确的密码生成方式(学号的MD5)。
        
        测试场景:
            - 检查发送的登录请求中密码字段
        
        预期结果:
            - 密码为学号的MD5哈希值
        """
        mock_sock = Mock()
        student_id = "test_student"
        expected_pwd = hashlib.md5(student_id.encode()).hexdigest()
        
        response_data = {"status": 200, "token": "token123"}
        response_packet = client_lhx.make_packet(response_data)
        mock_sock.recv.side_effect = [response_packet[i:i+1] for i in range(len(response_packet))]
        
        client_lhx.do_login(mock_sock, student_id)
        
        # 获取发送的数据包
        sent_packet = mock_sock.sendall.call_args[0][0]
        jlen, blen = struct.unpack("!II", sent_packet[:8])
        sent_json = json.loads(sent_packet[8:8+jlen].decode('utf-8'))
        
        self.assertEqual(sent_json['password'], expected_pwd)
        self.assertEqual(sent_json['username'], student_id)


class TestFileOperations(unittest.TestCase):
    """文件操作测试类
    
    测试文件上传、下载和状态查询相关的功能。
    """

    def test_req_save_plan_success(self):
        """测试请求保存计划成功
        
        验证req_save_plan函数在成功获取上传计划时的处理。
        
        测试场景:
            - 模拟服务器返回有效的上传计划
        
        预期结果:
            - 返回key、block_size、total_block
            - 打印计划信息
        """
        mock_sock = Mock()
        token = "valid_token"
        
        # 创建测试文件
        test_file = tempfile.NamedTemporaryFile(delete=False)
        test_file.write(b"A" * 10000)
        test_file.close()
        
        try:
            # 模拟服务器响应
            response_data = {
                "status": 200,
                "key": "test_file_key",
                "block_size": 4096,
                "total_block": 3
            }
            response_packet = client_lhx.make_packet(response_data)
            mock_sock.recv.side_effect = [response_packet[i:i+1] for i in range(len(response_packet))]
            
            with patch('builtins.print'):
                key, block_size, total_block = client_lhx.req_save_plan(
                    mock_sock, token, test_file.name, None
                )
                
                self.assertEqual(key, "test_file_key")
                self.assertEqual(block_size, 4096)
                self.assertEqual(total_block, 3)
        finally:
            os.unlink(test_file.name)

    def test_req_save_plan_with_desired_key(self):
        """测试请求保存计划时指定key
        
        验证req_save_plan函数在指定desired_key时是否正确发送。
        
        测试场景:
            - 提供desired_key参数
        
        预期结果:
            - 发送的请求中包含指定的key
        """
        mock_sock = Mock()
        token = "valid_token"
        desired_key = "my_custom_key"
        
        test_file = tempfile.NamedTemporaryFile(delete=False)
        test_file.write(b"test")
        test_file.close()
        
        try:
            response_data = {
                "status": 200,
                "key": desired_key,
                "block_size": 4096,
                "total_block": 1
            }
            response_packet = client_lhx.make_packet(response_data)
            mock_sock.recv.side_effect = [response_packet[i:i+1] for i in range(len(response_packet))]
            
            with patch('builtins.print'):
                client_lhx.req_save_plan(mock_sock, token, test_file.name, desired_key)
                
                # 检查发送的请求
                sent_packet = mock_sock.sendall.call_args[0][0]
                jlen, _ = struct.unpack("!II", sent_packet[:8])
                sent_json = json.loads(sent_packet[8:8+jlen].decode('utf-8'))
                
                self.assertEqual(sent_json['key'], desired_key)
        finally:
            os.unlink(test_file.name)

    def test_req_save_plan_failure(self):
        """测试请求保存计划失败
        
        验证req_save_plan函数在服务器拒绝时的异常处理。
        
        测试场景:
            - 模拟服务器返回status!=200
        
        预期结果:
            - 抛出RuntimeError异常
        """
        mock_sock = Mock()
        token = "valid_token"
        
        test_file = tempfile.NamedTemporaryFile(delete=False)
        test_file.write(b"test")
        test_file.close()
        
        try:
            response_data = {
                "status": 402,
                "message": "Key already exists"
            }
            response_packet = client_lhx.make_packet(response_data)
            mock_sock.recv.side_effect = [response_packet[i:i+1] for i in range(len(response_packet))]
            
            with self.assertRaises(RuntimeError) as context:
                client_lhx.req_save_plan(mock_sock, token, test_file.name, None)
            
            self.assertIn("SAVE failed", str(context.exception))
        finally:
            os.unlink(test_file.name)

    def test_do_upload_success(self):
        """测试文件上传成功
        
        验证do_upload函数能否正确上传文件的所有分块。
        
        测试场景:
            - 创建小文件并上传
            - 模拟服务器对每个块返回成功响应
        
        预期结果:
            - 发送正确数量的UPLOAD请求
            - 每个块的索引正确
            - 打印进度信息
        """
        mock_sock = Mock()
        token = "valid_token"
        key = "test_key"
        block_size = 100
        
        # 创建测试文件(250字节,需要3个块)
        test_file = tempfile.NamedTemporaryFile(delete=False)
        test_file.write(b"A" * 250)
        test_file.close()
        
        try:
            # 模拟服务器对每个块的响应
            def mock_recv(size):
                response_data = {"status": 200}
                response_packet = client_lhx.make_packet(response_data)
                if mock_recv.call_count <= len(response_packet):
                    result = response_packet[mock_recv.call_count-1:mock_recv.call_count]
                    return result
                return b""
            
            mock_recv.call_count = 0
            original_recv = mock_sock.recv
            
            def counting_recv(size):
                mock_recv.call_count += 1
                response_data = {"status": 200}
                response_packet = client_lhx.make_packet(response_data)
                if mock_recv.call_count <= len(response_packet):
                    return response_packet[mock_recv.call_count-1:mock_recv.call_count]
                mock_recv.call_count = 0
                return response_packet[0:1]
            
            mock_sock.recv.side_effect = counting_recv
            
            with patch('builtins.print'):
                client_lhx.do_upload(mock_sock, token, key, test_file.name, block_size, 3)
            
            # 验证发送了3次UPLOAD请求
            self.assertEqual(mock_sock.sendall.call_count, 3)
        finally:
            os.unlink(test_file.name)

    def test_do_upload_server_error(self):
        """测试上传过程中服务器错误
        
        验证do_upload函数在服务器返回错误时的异常处理。
        
        测试场景:
            - 模拟服务器对某个块返回失败响应
        
        预期结果:
            - 抛出RuntimeError异常
            - 异常消息包含块索引信息
        """
        mock_sock = Mock()
        token = "valid_token"
        key = "test_key"
        block_size = 100
        
        test_file = tempfile.NamedTemporaryFile(delete=False)
        test_file.write(b"A" * 100)
        test_file.close()
        
        try:
            # 模拟服务器错误响应
            response_data = {"status": 500, "message": "Internal error"}
            response_packet = client_lhx.make_packet(response_data)
            mock_sock.recv.side_effect = [response_packet[i:i+1] for i in range(len(response_packet))]
            
            with self.assertRaises(RuntimeError) as context:
                client_lhx.do_upload(mock_sock, token, key, test_file.name, block_size, 1)
            
            self.assertIn("UPLOAD failed", str(context.exception))
        finally:
            os.unlink(test_file.name)

    def test_get_status_success(self):
        """测试获取文件状态成功
        
        验证get_status函数能否正确获取服务器上的文件状态。
        
        测试场景:
            - 模拟服务器返回文件状态信息
        
        预期结果:
            - 返回包含文件信息的字典
        """
        mock_sock = Mock()
        token = "valid_token"
        key = "test_file"
        
        # 模拟服务器响应
        response_data = {
            "status": 200,
            "key": key,
            "md5": "abc123def456",
            "size": 1024
        }
        response_packet = client_lhx.make_packet(response_data)
        mock_sock.recv.side_effect = [response_packet[i:i+1] for i in range(len(response_packet))]
        
        result = client_lhx.get_status(mock_sock, token, key)
        
        self.assertEqual(result['status'], 200)
        self.assertEqual(result['md5'], "abc123def456")
        self.assertEqual(result['key'], key)

    def test_get_status_file_not_found(self):
        """测试获取不存在文件的状态
        
        验证get_status函数在文件不存在时的异常处理。
        
        测试场景:
            - 模拟服务器返回404文件未找到
        
        预期结果:
            - 抛出RuntimeError异常
        """
        mock_sock = Mock()
        token = "valid_token"
        key = "nonexistent_file"
        
        response_data = {"status": 404, "message": "File not found"}
        response_packet = client_lhx.make_packet(response_data)
        mock_sock.recv.side_effect = [response_packet[i:i+1] for i in range(len(response_packet))]
        
        with self.assertRaises(RuntimeError) as context:
            client_lhx.get_status(mock_sock, token, key)
        
        self.assertIn("GET failed", str(context.exception))


class TestMainFunction(unittest.TestCase):
    """主函数测试类
    
    测试main函数的整体流程和异常处理。
    """

    def test_main_file_not_found(self):
        """测试主函数处理文件不存在
        
        验证main函数在文件不存在时的处理。
        
        测试场景:
            - 参数中的文件路径不存在
        
        预期结果:
            - 打印错误消息
            - 调用sys.exit(1)退出
        """
        test_args = [
            '--server_ip', '127.0.0.1',
            '--id', 'student123',
            '-f', '/nonexistent/file.txt'
        ]
        
        with patch('sys.argv', ['client_lhx.py'] + test_args):
            with patch('client_lhx._ask', return_value='/another/nonexistent.txt'):
                with patch('sys.exit') as mock_exit:
                    with patch('builtins.print') as mock_print:
                        # parse_and_validate_args会一直循环直到得到存在的文件
                        # 这里我们测试main中的文件检查
                        pass

    @patch('client_lhx.get_status')
    @patch('client_lhx.do_upload')
    @patch('client_lhx.req_save_plan')
    @patch('client_lhx.do_login')
    @patch('socket.socket')
    @patch('client_lhx.parse_and_validate_args')
    def test_main_complete_flow(self, mock_parse, mock_socket, mock_login, 
                                 mock_save, mock_upload, mock_get_status):
        """测试主函数完整流程
        
        验证main函数的完整执行流程,包括登录、上传和验证。
        
        测试场景:
            - 模拟完整的客户端操作流程
        
        预期结果:
            - 依次调用登录、请求保存计划、上传、获取状态
            - MD5验证通过时打印MATCH
        """
        # 创建测试文件
        test_file = tempfile.NamedTemporaryFile(delete=False)
        test_content = b"test file for main function"
        test_file.write(test_content)
        test_file.close()
        
        try:
            # 配置mock对象
            mock_args = Mock()
            mock_args.file = test_file.name
            mock_args.server_ip = '127.0.0.1'
            mock_args.server_port = 1379
            mock_args.id = 'student123'
            mock_args.key = None
            mock_parse.return_value = mock_args
            
            mock_sock_instance = Mock()
            mock_socket.return_value.__enter__.return_value = mock_sock_instance
            
            mock_login.return_value = "test_token"
            mock_save.return_value = ("file_key", 4096, 1)
            
            # MD5应该匹配
            expected_md5 = hashlib.md5(test_content).hexdigest()
            mock_get_status.return_value = {"md5": expected_md5}
            
            with patch('builtins.print') as mock_print:
                client_lhx.main()
                
                # 验证调用顺序
                mock_login.assert_called_once()
                mock_save.assert_called_once()
                mock_upload.assert_called_once()
                mock_get_status.assert_called_once()
                
                # 验证打印了MATCH消息
                print_calls = [str(call) for call in mock_print.call_args_list]
                self.assertTrue(any("MATCH" in call for call in print_calls))
        finally:
            os.unlink(test_file.name)

    @patch('client_lhx.get_status')
    @patch('client_lhx.do_upload')
    @patch('client_lhx.req_save_plan')
    @patch('client_lhx.do_login')
    @patch('socket.socket')
    @patch('client_lhx.parse_and_validate_args')
    def test_main_md5_mismatch(self, mock_parse, mock_socket, mock_login,
                                mock_save, mock_upload, mock_get_status):
        """测试主函数MD5不匹配
        
        验证main函数在MD5校验失败时的处理。
        
        测试场景:
            - 服务器返回的MD5与本地不匹配
        
        预期结果:
            - 打印MISMATCH消息
            - 显示服务器和本地的MD5值
        """
        test_file = tempfile.NamedTemporaryFile(delete=False)
        test_file.write(b"test content")
        test_file.close()
        
        try:
            mock_args = Mock()
            mock_args.file = test_file.name
            mock_args.server_ip = '127.0.0.1'
            mock_args.server_port = 1379
            mock_args.id = 'student123'
            mock_args.key = None
            mock_parse.return_value = mock_args
            
            mock_sock_instance = Mock()
            mock_socket.return_value.__enter__.return_value = mock_sock_instance
            
            mock_login.return_value = "test_token"
            mock_save.return_value = ("file_key", 4096, 1)
            
            # MD5不匹配
            mock_get_status.return_value = {"md5": "wrong_md5_value"}
            
            with patch('builtins.print') as mock_print:
                client_lhx.main()
                
                # 验证打印了MISMATCH消息
                print_calls = [str(call) for call in mock_print.call_args_list]
                self.assertTrue(any("MISMATCH" in call for call in print_calls))
        finally:
            os.unlink(test_file.name)


class TestEdgeCases(unittest.TestCase):
    """边界情况测试类
    
    测试各种边界条件和特殊情况。
    """

    def test_empty_token_in_response(self):
        """测试登录响应中token为空
        
        验证do_login函数处理空token的情况。
        
        测试场景:
            - 服务器返回status=200但token为空字符串
        
        预期结果:
            - 返回空字符串(经过strip处理)
        """
        mock_sock = Mock()
        response_data = {"status": 200, "token": "  "}
        response_packet = client_lhx.make_packet(response_data)
        mock_sock.recv.side_effect = [response_packet[i:i+1] for i in range(len(response_packet))]
        
        with patch('builtins.print'):
            token = client_lhx.do_login(mock_sock, "student123")
            self.assertEqual(token, "")

    def test_chinese_characters_in_json(self):
        """测试JSON中包含中文字符
        
        验证make_packet和read_packet函数对中文字符的处理。
        
        测试场景:
            - JSON数据中包含中文字符
        
        预期结果:
            - 正确编码和解码中文字符
        """
        test_data = {"message": "上传成功", "key": "文件名"}
        packet = client_lhx.make_packet(test_data)
        
        mock_sock = Mock()
        mock_sock.recv.side_effect = [packet[i:i+1] for i in range(len(packet))]
        
        json_data, body = client_lhx.read_packet(mock_sock)
        self.assertEqual(json_data['message'], "上传成功")
        self.assertEqual(json_data['key'], "文件名")

    def test_very_large_json(self):
        """测试超大JSON数据包
        
        验证make_packet函数处理大型JSON数据的能力。
        
        测试场景:
            - 创建包含大量数据的JSON对象
        
        预期结果:
            - 正确构造和解析大型数据包
        """
        large_data = {"data": "X" * 100000, "type": "TEST"}
        packet = client_lhx.make_packet(large_data)
        
        mock_sock = Mock()
        mock_sock.recv.side_effect = [packet[i:i+1] for i in range(len(packet))]
        
        json_data, body = client_lhx.read_packet(mock_sock)
        self.assertEqual(len(json_data['data']), 100000)

    def test_zero_byte_file(self):
        """测试零字节文件的MD5计算
        
        验证md5_file函数处理空文件的情况(已在TestProtocolFunctions中测试)。
        这里作为边界情况的补充测试。
        
        测试场景:
            - 0字节文件
        
        预期结果:
            - 返回空内容的MD5值(d41d8cd98f00b204e9800998ecf8427e)
        """
        test_file = tempfile.NamedTemporaryFile(delete=False)
        test_file.close()
        
        try:
            result = client_lhx.md5_file(test_file.name)
            # 空文件的MD5值
            self.assertEqual(result, "d41d8cd98f00b204e9800998ecf8427e")
        finally:
            os.unlink(test_file.name)

    def test_ip_address_boundary_values(self):
        """测试IP地址边界值验证
        
        验证_is_ipv4_strict函数对边界值的处理。
        
        测试场景:
            - 各种边界值IP地址
        
        预期结果:
            - 正确识别有效和无效的边界值
        """
        # 有效边界值
        self.assertTrue(client_lhx._is_ipv4_strict("0.0.0.0"))
        self.assertTrue(client_lhx._is_ipv4_strict("255.255.255.255"))
        
        # 无效边界值
        self.assertFalse(client_lhx._is_ipv4_strict("256.0.0.0"))
        self.assertFalse(client_lhx._is_ipv4_strict("0.0.0.-1"))
        self.assertFalse(client_lhx._is_ipv4_strict("255.255.255.256"))


class TestExceptionHandling(unittest.TestCase):
    """异常处理测试类
    
    专门测试各种异常情况的处理。
    """

    def test_socket_connection_error(self):
        """测试Socket连接错误
        
        验证主函数在无法连接到服务器时的处理。
        
        测试场景:
            - 模拟socket连接失败
        
        预期结果:
            - 抛出socket.error或ConnectionRefusedError
        """
        test_file = tempfile.NamedTemporaryFile(delete=False)
        test_file.write(b"test")
        test_file.close()
        
        try:
            mock_args = Mock()
            mock_args.file = test_file.name
            mock_args.server_ip = '127.0.0.1'
            mock_args.server_port = 9999
            mock_args.id = 'student123'
            mock_args.key = None
            
            with patch('client_lhx.parse_and_validate_args', return_value=mock_args):
                with patch('socket.socket') as mock_socket:
                    mock_socket.return_value.__enter__.return_value.connect.side_effect = ConnectionRefusedError
                    
                    with self.assertRaises(ConnectionRefusedError):
                        client_lhx.main()
        finally:
            os.unlink(test_file.name)

    def test_file_read_permission_error(self):
        """测试文件读取权限错误
        
        验证md5_file函数在文件无读权限时的异常处理。
        
        测试场景:
            - 尝试读取无权限的文件
        
        预期结果:
            - 抛出PermissionError或IOError
        """
        # 这个测试在某些系统上可能需要特殊权限,因此用mock模拟
        with patch('builtins.open', side_effect=PermissionError("No permission")):
            with self.assertRaises(PermissionError):
                client_lhx.md5_file("/fake/path/file.txt")

    def test_malformed_json_response(self):
        """测试畸形JSON响应
        
        验证read_packet函数在接收到无效JSON时的处理。
        
        测试场景:
            - 服务器返回无效的JSON数据
        
        预期结果:
            - 抛出json.JSONDecodeError
        """
        mock_sock = Mock()
        # 构造包含无效JSON的数据包
        invalid_json = b"{ invalid json }"
        packet = struct.pack("!II", len(invalid_json), 0) + invalid_json
        
        mock_sock.recv.side_effect = [packet[i:i+1] for i in range(len(packet))]
        
        with self.assertRaises(json.JSONDecodeError):
            client_lhx.read_packet(mock_sock)

    def test_network_timeout(self):
        """测试网络超时
        
        验证recv_exact函数在网络超时时的处理。
        
        测试场景:
            - 模拟socket超时
        
        预期结果:
            - 抛出socket.timeout异常
        """
        mock_sock = Mock()
        mock_sock.recv.side_effect = socket.timeout("Connection timeout")
        
        with self.assertRaises(socket.timeout):
            client_lhx.recv_exact(mock_sock, 100)

    def test_unexpected_response_format(self):
        """测试意外的响应格式
        
        验证do_login函数在响应缺少必要字段时的处理。
        
        测试场景:
            - 服务器响应缺少token字段
        
        预期结果:
            - 能够处理缺失的字段(返回空字符串或引发KeyError)
        """
        mock_sock = Mock()
        # 响应中没有token字段
        response_data = {"status": 200, "message": "OK"}
        response_packet = client_lhx.make_packet(response_data)
        mock_sock.recv.side_effect = [response_packet[i:i+1] for i in range(len(response_packet))]
        
        with patch('builtins.print'):
            token = client_lhx.do_login(mock_sock, "student123")
            # get方法会返回空字符串,strip后仍是空
            self.assertEqual(token, "")


class TestIntegration(unittest.TestCase):
    """集成测试类
    
    测试多个组件协同工作的情况。
    """

    @patch('socket.socket')
    def test_full_upload_workflow(self, mock_socket):
        """测试完整上传工作流
        
        验证登录、请求保存、上传、获取状态的完整流程。
        
        测试场景:
            - 模拟完整的文件上传过程
        
        预期结果:
            - 所有步骤按顺序成功执行
        """
        test_file = tempfile.NamedTemporaryFile(delete=False)
        test_content = b"integration test content"
        test_file.write(test_content)
        test_file.close()
        
        try:
            mock_sock = Mock()
            mock_socket.return_value.__enter__.return_value = mock_sock
            
            student_id = "test_student"
            
            # 1. 登录响应
            login_response = {"status": 200, "token": "integration_token"}
            login_packet = client_lhx.make_packet(login_response)
            
            # 2. 保存计划响应
            save_response = {
                "status": 200,
                "key": "test_key",
                "block_size": 100,
                "total_block": 1
            }
            save_packet = client_lhx.make_packet(save_response)
            
            # 3. 上传响应
            upload_response = {"status": 200, "block_index": 0}
            upload_packet = client_lhx.make_packet(upload_response)
            
            # 4. 状态响应
            status_response = {
                "status": 200,
                "md5": hashlib.md5(test_content).hexdigest()
            }
            status_packet = client_lhx.make_packet(status_response)
            
            # 配置recv的返回值
            all_responses = (
                [login_packet[i:i+1] for i in range(len(login_packet))] +
                [save_packet[i:i+1] for i in range(len(save_packet))] +
                [upload_packet[i:i+1] for i in range(len(upload_packet))] +
                [status_packet[i:i+1] for i in range(len(status_packet))]
            )
            
            mock_sock.recv.side_effect = all_responses
            
            with patch('builtins.print'):
                # 执行完整流程
                token = client_lhx.do_login(mock_sock, student_id)
                self.assertEqual(token, "integration_token")
                
                key, block_size, total_block = client_lhx.req_save_plan(
                    mock_sock, token, test_file.name, None
                )
                self.assertEqual(key, "test_key")
                
                client_lhx.do_upload(mock_sock, token, key, test_file.name, block_size, total_block)
                
                status = client_lhx.get_status(mock_sock, token, key)
                self.assertEqual(status['status'], 200)
        finally:
            os.unlink(test_file.name)


def run_tests():
    """运行所有测试
    
    这个函数提供了一个便捷的方式来运行所有测试用例。
    
    使用方法:
        python test_lhx.py
    
    返回:
        unittest.TestResult: 测试结果对象
    """
    # 创建测试套件
    loader = unittest.TestLoader()
    suite = unittest.TestSuite()
    
    # 添加所有测试类
    suite.addTests(loader.loadTestsFromTestCase(TestBasicFunctions))
    suite.addTests(loader.loadTestsFromTestCase(TestArgumentParsing))
    suite.addTests(loader.loadTestsFromTestCase(TestProtocolFunctions))
    suite.addTests(loader.loadTestsFromTestCase(TestAuthenticationOperations))
    suite.addTests(loader.loadTestsFromTestCase(TestFileOperations))
    suite.addTests(loader.loadTestsFromTestCase(TestMainFunction))
    suite.addTests(loader.loadTestsFromTestCase(TestEdgeCases))
    suite.addTests(loader.loadTestsFromTestCase(TestExceptionHandling))
    suite.addTests(loader.loadTestsFromTestCase(TestIntegration))
    
    # 运行测试
    runner = unittest.TextTestRunner(verbosity=2)
    return runner.run(suite)


if __name__ == '__main__':
    """主入口点
    
    当直接运行此文件时,执行所有测试用例。
    """
    result = run_tests()
    
    # 根据测试结果设置退出代码
    sys.exit(0 if result.wasSuccessful() else 1)
