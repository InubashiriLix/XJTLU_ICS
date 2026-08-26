;; 练习 3 参考解法:strlen
(module
  (memory 1)
  (data (i32.const 0) "hi wasm")
  (func (export "strlen") (param $base i32) (result i32)
    (local $i i32)
    i32.const 0
    local.set $i
    block $done
      loop $loop
        local.get $base
        local.get $i
        i32.add
        i32.load8_u
        i32.eqz
        br_if $done
        local.get $i
        i32.const 1
        i32.add
        local.set $i
        br $loop
      end
    end
    local.get $i))
