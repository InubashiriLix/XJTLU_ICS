;; 练习 1 参考解法:导入打印 + 导出 twice
(module
  (import "env" "print_i32" (func $print (param i32)))
  (func (export "twice") (param $n i32) (result i32)
    (local $t i32)
    local.get $n
    i32.const 2
    i32.mul
    local.tee $t
    call $print
    local.get $t))
