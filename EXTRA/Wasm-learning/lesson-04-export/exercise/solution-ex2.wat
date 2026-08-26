;; 练习 2 参考解法:打印内存字符串
(module
  (import "env" "print_str" (func $print_str (param i32) (param i32)))
  (memory 1)
  (data (i32.const 0) "string in wasm")
  (func (export "print_str2")
    i32.const 0
    i32.const 14
    call $print_str))
