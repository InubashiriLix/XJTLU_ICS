;; 练习 1 参考解法:(2 + 5) * 4 = 28
(module
  (func (export "calc") (result i32)
    i32.const 2
    i32.const 5
    i32.add
    i32.const 4
    i32.mul))
