(module
  ;; 从宿主导入打印函数
  (import "env" "print_i32" (func $print (param i32)))
  (import "env" "print_str" (func $print_str (param i32) (param i32)))

  (memory (export "mem") 1)

  ;; 一段字符串放内存里
  (data (i32.const 0) "hello from wasm")

  ;; 导出:打印一个整数
  (func (export "say_num") (param $n i32)
    local.get $n
    call $print)

  ;; 导出:打印内存里的字符串
  (func (export "say_hi")
    i32.const 0      ;; 字符串起始地址
    i32.const 14     ;; 字符串长度("hello from wasm" 是 14 个字符)
    call $print_str)

  ;; 导出:简单加法,顺便打印过程
  (func (export "add_and_print") (param $a i32) (param $b i32) (result i32)
    (local $t i32)
    local.get $a
    local.get $b
    i32.add
    local.tee $t      ;; 结果存进 $t 同时留在栈上
    call $print
    local.get $t))
