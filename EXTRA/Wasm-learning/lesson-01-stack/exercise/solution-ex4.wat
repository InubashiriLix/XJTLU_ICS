;; 练习 4 思考:用负数试试 div_s vs div_u
(module
  (func (export "ds") (result i32)
    i32.const -7
    i32.const 2
    i32.div_s)   ;; 有符号除法:-7/2 = -3
  (func (export "du") (result i32)
    i32.const -7
    i32.const 2
    i32.div_u))  ;; 无符号除法:把 -7 当 4294967289,结果巨大
