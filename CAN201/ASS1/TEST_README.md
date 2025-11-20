# test_lhx.py 测试文档

## 概述

`test_lhx.py` 是针对 `client_lhx.py` 的全面测试套件，使用Python的unittest框架编写，包含详细的Google风格中文文档注释。

## 测试覆盖范围

### 1. TestBasicFunctions (基础工具函数测试类)
测试client_lhx.py中的基础工具函数。

**测试用例:**
- `test_ask_normal_input` - 测试正常输入处理
- `test_ask_eoferror` - 测试EOF异常处理
- `test_normalize_ip_localhost` - 测试localhost到127.0.0.1的转换
- `test_normalize_ip_standard` - 测试标准IP地址规范化
- `test_is_ipv4_strict_valid` - 测试有效IPv4地址验证
- `test_is_ipv4_strict_invalid` - 测试无效IPv4地址拒绝

### 2. TestArgumentParsing (命令行参数解析测试类)
测试命令行参数的解析和验证功能。

**测试用例:**
- `test_parse_and_validate_args_complete` - 测试完整参数解析
- `test_parse_and_validate_args_invalid_ip_then_valid` - 测试无效IP后提供有效IP
- `test_parse_and_validate_args_missing_id` - 测试缺少学号参数
- `test_parse_and_validate_args_nonexistent_file` - 测试文件不存在的情况

### 3. TestProtocolFunctions (STEP协议通信函数测试类)
测试STEP协议相关的数据包构造、接收和解析功能。

**测试用例:**
- `test_make_packet_json_only` - 测试仅包含JSON的数据包构造
- `test_make_packet_with_body` - 测试包含二进制数据的数据包构造
- `test_recv_exact_success` - 测试正常接收指定长度数据
- `test_recv_exact_partial_receive` - 测试分段接收数据
- `test_recv_exact_connection_closed` - 测试连接关闭异常
- `test_read_packet_json_only` - 测试读取纯JSON数据包
- `test_read_packet_with_body` - 测试读取包含二进制数据的数据包
- `test_md5_file` - 测试MD5哈希值计算
- `test_md5_file_large_file` - 测试大文件MD5计算
- `test_md5_file_empty_file` - 测试空文件MD5计算

### 4. TestAuthenticationOperations (认证操作测试类)
测试登录认证相关功能。

**测试用例:**
- `test_do_login_success` - 测试登录成功场景
- `test_do_login_failure` - 测试登录失败场景
- `test_do_login_password_generation` - 测试密码生成逻辑

### 5. TestFileOperations (文件操作测试类)
测试文件上传、下载和状态查询相关功能。

**测试用例:**
- `test_req_save_plan_success` - 测试请求保存计划成功
- `test_req_save_plan_with_desired_key` - 测试指定key的保存计划
- `test_req_save_plan_failure` - 测试请求保存计划失败
- `test_do_upload_success` - 测试文件上传成功
- `test_do_upload_server_error` - 测试上传过程中服务器错误
- `test_get_status_success` - 测试获取文件状态成功
- `test_get_status_file_not_found` - 测试获取不存在文件的状态

### 6. TestMainFunction (主函数测试类)
测试main函数的整体流程和异常处理。

**测试用例:**
- `test_main_file_not_found` - 测试文件不存在的处理
- `test_main_complete_flow` - 测试完整流程
- `test_main_md5_mismatch` - 测试MD5不匹配

### 7. TestEdgeCases (边界情况测试类)
测试各种边界条件和特殊情况。

**测试用例:**
- `test_empty_token_in_response` - 测试空token处理
- `test_chinese_characters_in_json` - 测试中文字符处理
- `test_very_large_json` - 测试超大JSON数据包
- `test_zero_byte_file` - 测试零字节文件
- `test_ip_address_boundary_values` - 测试IP地址边界值

### 8. TestExceptionHandling (异常处理测试类)
专门测试各种异常情况的处理。

**测试用例:**
- `test_socket_connection_error` - 测试Socket连接错误
- `test_file_read_permission_error` - 测试文件读取权限错误
- `test_malformed_json_response` - 测试畸形JSON响应
- `test_network_timeout` - 测试网络超时
- `test_unexpected_response_format` - 测试意外的响应格式

### 9. TestIntegration (集成测试类)
测试多个组件协同工作的情况。

**测试用例:**
- `test_full_upload_workflow` - 测试完整上传工作流

## 运行测试

### 运行所有测试
```bash
python3 -m pytest test_lhx.py -v
```

### 运行特定测试类
```bash
python3 -m pytest test_lhx.py::TestBasicFunctions -v
```

### 运行特定测试用例
```bash
python3 -m pytest test_lhx.py::TestBasicFunctions::test_normalize_ip -v
```

### 使用unittest运行
```bash
python3 test_lhx.py
```

### 查看测试覆盖率
```bash
python3 -m pytest test_lhx.py --cov=client_lhx --cov-report=html
```

## 依赖项

测试需要以下Python包:
- unittest (标准库)
- unittest.mock (标准库)
- pytest (可选，用于更好的测试运行体验)
- tqdm (client_lhx.py的依赖)

安装依赖:
```bash
pip3 install pytest tqdm
```

## 测试特点

### 1. 详细的中文Google风格文档
每个测试用例都包含:
- 简短描述
- 详细说明验证的功能
- 测试场景说明
- 预期结果说明

### 2. 全面的异常捕获测试
针对以下异常情况进行测试:
- EOFError (输入流结束)
- ConnectionError (连接异常)
- RuntimeError (运行时错误)
- PermissionError (权限错误)
- json.JSONDecodeError (JSON解析错误)
- socket.timeout (网络超时)

### 3. Mock对象使用
广泛使用Mock对象来:
- 模拟socket连接
- 模拟文件系统操作
- 模拟用户输入
- 隔离测试环境

### 4. 边界条件测试
包括:
- 空文件
- 超大文件
- IP地址边界值(0.0.0.0, 255.255.255.255)
- 空字符串
- 中文字符
- 超大JSON数据

### 5. 集成测试
测试完整的客户端工作流:
- 登录 → 请求保存计划 → 上传文件 → 验证MD5

## 测试结果示例

```
============================= test session starts ==============================
platform linux -- Python 3.10.12, pytest-6.2.5
collected 44 items

test_lhx.py::TestBasicFunctions::test_ask_eoferror PASSED                [  2%]
test_lhx.py::TestBasicFunctions::test_ask_normal_input PASSED            [  4%]
test_lhx.py::TestBasicFunctions::test_is_ipv4_strict_invalid PASSED      [  6%]
...
test_lhx.py::TestIntegration::test_full_upload_workflow PASSED           [100%]

============================== 44 passed in 2.34s ===============================
```

## 注意事项

1. 某些测试会创建临时文件，测试结束后会自动清理
2. 测试使用Mock对象，不会实际连接到服务器
3. 所有测试都是独立的，可以单独运行
4. 测试覆盖了正常流程和异常情况

## 维护建议

1. 当client_lhx.py添加新功能时，应同步添加相应测试
2. 保持中文文档的更新和详细
3. 确保所有异常情况都有对应的测试用例
4. 定期运行完整测试套件确保代码质量
