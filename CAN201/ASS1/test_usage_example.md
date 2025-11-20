# test_lhx.py 使用示例

## 快速开始

### 1. 运行所有测试
```bash
# 使用pytest (推荐)
python3 -m pytest test_lhx.py -v

# 使用unittest
python3 test_lhx.py
```

### 2. 运行特定测试类
```bash
# 只测试基础函数
python3 -m pytest test_lhx.py::TestBasicFunctions -v

# 只测试异常处理
python3 -m pytest test_lhx.py::TestExceptionHandling -v
```

### 3. 运行单个测试用例
```bash
# 测试IP验证功能
python3 -m pytest test_lhx.py::TestBasicFunctions::test_is_ipv4_strict_valid -v

# 测试登录功能
python3 -m pytest test_lhx.py::TestAuthenticationOperations::test_do_login_success -v
```

## 测试输出示例

### 成功的测试输出
```
test_lhx.py::TestBasicFunctions::test_normalize_ip_localhost PASSED [100%]

============================== 1 passed in 0.02s ===============================
```

### 失败的测试输出
```
test_lhx.py::TestBasicFunctions::test_example FAILED [100%]

================================== FAILURES ====================================
________________________________ test_example __________________________________

    def test_example(self):
>       self.assertEqual(result, expected)
E       AssertionError: 'actual' != 'expected'

test_lhx.py:123: AssertionError
============================== 1 failed in 0.03s ===============================
```

## 查看测试覆盖率

```bash
# 安装coverage工具
pip3 install pytest-cov

# 运行测试并生成覆盖率报告
python3 -m pytest test_lhx.py --cov=client_lhx --cov-report=term-missing

# 生成HTML格式的覆盖率报告
python3 -m pytest test_lhx.py --cov=client_lhx --cov-report=html
# 然后在浏览器中打开 htmlcov/index.html
```

## 测试类别说明

| 测试类 | 用例数 | 主要测试内容 |
|--------|--------|------------|
| TestBasicFunctions | 6 | 基础工具函数：IP验证、输入处理等 |
| TestArgumentParsing | 4 | 命令行参数解析和验证 |
| TestProtocolFunctions | 10 | STEP协议数据包构造和解析 |
| TestAuthenticationOperations | 3 | 登录认证功能 |
| TestFileOperations | 7 | 文件上传、下载、状态查询 |
| TestMainFunction | 3 | 主函数完整流程 |
| TestEdgeCases | 5 | 边界情况和特殊输入 |
| TestExceptionHandling | 5 | 异常处理和错误恢复 |
| TestIntegration | 1 | 端到端集成测试 |
| **总计** | **44** | **完整的功能和异常覆盖** |

## 调试测试

### 查看详细输出
```bash
# 显示print语句输出
python3 -m pytest test_lhx.py -v -s

# 显示更详细的失败信息
python3 -m pytest test_lhx.py -v --tb=long
```

### 运行失败时停止
```bash
# 遇到第一个失败就停止
python3 -m pytest test_lhx.py -x

# 遇到3个失败后停止
python3 -m pytest test_lhx.py --maxfail=3
```

### 只运行失败的测试
```bash
# 先运行一次记录失败
python3 -m pytest test_lhx.py

# 只重新运行失败的测试
python3 -m pytest test_lhx.py --lf
```

## 常见问题

### Q: ModuleNotFoundError: No module named 'tqdm'
**A:** 安装依赖包
```bash
pip3 install tqdm
```

### Q: 如何跳过某个测试？
**A:** 在测试函数上添加装饰器
```python
@unittest.skip("暂时跳过此测试")
def test_something(self):
    pass
```

### Q: 如何只运行标记的测试？
**A:** 使用pytest的标记功能
```python
import pytest

@pytest.mark.slow
def test_slow_operation(self):
    pass
```

然后运行：
```bash
python3 -m pytest test_lhx.py -v -m slow
```

## 扩展测试

如果需要添加新的测试用例，遵循以下模板：

```python
def test_新功能_场景描述(self):
    """测试新功能的某个场景
    
    验证新功能在特定条件下的行为。
    
    测试场景:
        - 详细描述测试场景
    
    预期结果:
        - 描述预期的结果
    """
    # 准备测试数据
    test_data = "测试数据"
    
    # 执行被测试的函数
    result = client_lhx.some_function(test_data)
    
    # 验证结果
    self.assertEqual(result, expected_value)
```

## 性能提示

- 使用pytest会比unittest快
- 并行运行测试：`pip3 install pytest-xdist` 然后 `pytest -n 4`
- 只运行必要的测试类以节省时间
