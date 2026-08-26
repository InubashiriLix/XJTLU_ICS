(module
  (memory 1)

  ;; 地址 0..3:小端序整数 42
  (data (i32.const 0) "\2A\00\00\00")

  ;; 把 v 写入地址 0
  (func (export "set0") (param $v i32)
    i32.const 0
    local.get $v
    i32.store)

  ;; 读地址 0 的值
  (func (export "get0") (result i32)
    i32.const 0
    i32.load)

  ;; 地址 16..31:数组 [10, 20, 30, 40]
  (data (i32.const 16) "\0A\00\00\00\14\00\00\00\1E\00\00\00\28\00\00\00")

  ;; 数组元素是 i32(4 字节),第 i 个元素地址 = base + i*4
  (func (export "sum") (param $base i32) (param $len i32) (result i32)
    (local $i i32)
    (local $sum i32)
    i32.const 0
    local.set $i
    i32.const 0
    local.set $sum
    block $done
      loop $loop
        local.get $i
        local.get $len
        i32.ge_s
        br_if $done
        local.get $base
        local.get $i
        i32.const 4
        i32.mul
        i32.add
        i32.load
        local.get $sum
        i32.add
        local.set $sum
        local.get $i
        i32.const 1
        i32.add
        local.set $i
        br $loop
      end
    end
    local.get $sum))
