(module
  (func (export "fact") (param $n i32) (result i32)
    (local $i i32)
    (local $acc i32)
    i32.const 1
    local.set $acc
    i32.const 1
    local.set $i
    block $done
      loop $loop
        local.get $i
        local.get $n
        i32.gt_s
        br_if $done
        local.get $acc
        local.get $i
        i32.mul
        local.set $acc
        local.get $i
        i32.const 1
        i32.add
        local.set $i
        br $loop
      end
    end
    local.get $acc)

  (func (export "add2") (param $a i32) (param $b i32) (result i32)
    local.get $a
    local.get $b
    i32.add)

  (func (export "abs") (param $x i32) (result i32)
    local.get $x
    i32.const 0
    i32.lt_s
    if (result i32)
      i32.const 0
      local.get $x
      i32.sub       ;; 0 - x = -x
    else
      local.get $x
    end)

  (func (export "count") (param $n i32) (result i32)
    (local $i i32)
    (local $sum i32)
    i32.const 0
    local.set $i
    i32.const 0
    local.set $sum
    block $done
      loop $again
        local.get $i
        local.get $n
        i32.ge_s
        br_if $done
        local.get $i
        local.get $sum
        i32.add
        local.set $sum
        local.get $i
        i32.const 1
        i32.add
        local.set $i
        br $again
      end
    end
    local.get $sum))
