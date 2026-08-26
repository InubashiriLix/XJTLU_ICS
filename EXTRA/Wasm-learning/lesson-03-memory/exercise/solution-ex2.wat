;; 练习 2 参考解法:字节读写 + _s/_u 区别
(module
  (memory 1)
  (func (export "store") (param $v i32)
    i32.const 0
    local.get $v
    i32.store8)
  (func (export "read_u") (result i32)
    i32.const 0
    i32.load8_u)
  (func (export "read_s") (result i32)
    i32.const 0
    i32.load8_s))
