;; 练习 3 参考解法:report 分三行
(module
  (import "env" "print_i32" (func $print (param i32)))
  (func (export "report") (param $a i32) (param $b i32) (result i32)
    (local $sum i32)
    local.get $a
    call $print
    local.get $b
    call $print
    local.get $a
    local.get $b
    i32.add
    local.set $sum
    local.get $sum
    call $print
    local.get $sum))
