(module
  (func (export "calc") (result i32)
    i32.const 1
    i32.const 2
    i32.add
    i32.const 3
    i32.mul)
  (func (export "divmod") (result i32)
    i32.const 17
    i32.const 5
    i32.div_s)
  (func (export "rem") (result i32)
    i32.const 17
    i32.const 5
    i32.rem_s)
  (func (export "area") (result f64)
    f64.const 2.0
    f64.const 2.0
    f64.mul
    f64.const 3.14159
    f64.mul))
