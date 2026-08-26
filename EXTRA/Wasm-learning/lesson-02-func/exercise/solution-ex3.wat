;; 练习 3 参考解法:fib(循环版)
(module
  (func (export "fib") (param $n i32) (result i32)
    (local $prev i32)
    (local $curr i32)
    (local $tmp i32)
    (local $i i32)
    i32.const 0
    local.set $prev
    i32.const 1
    local.set $curr
    i32.const 1
    local.set $i
    block $done
      loop $loop
        local.get $i
        local.get $n
        i32.ge_s
        br_if $done
        local.get $curr
        local.set $tmp
        local.get $curr
        local.get $prev
        i32.add
        local.set $curr
        local.get $tmp
        local.set $prev
        local.get $i
        i32.const 1
        i32.add
        local.set $i
        br $loop
      end
    end
    local.get $curr))
