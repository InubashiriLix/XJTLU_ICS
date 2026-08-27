(; (module ;)
(;     (func (export "answer") ;)
(;         i32.const 7)) ;)
(;  wat2wasm exercise3.wat  ;)
(; exercise3.wat:3:9: error: type mismatch at end of function, expected [] but got [i32] ;)
(;         i32.const 7)) ;)
(;         ^^^^^^^^^ ;)

(module
    (func (export "nope")))
