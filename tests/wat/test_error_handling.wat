(module
  (type (;0;) (func))
  (import "test" "external_call" (func (;0;) (type 0)))
  (import "test" "good_call" (func (;0;) (type 0)))


  (func (export "call1") (result i32)
    call 0
    i32.const 0)

  (func (export "call2") (result i32)
    call 1
    i32.const 1)

  (func (export "unreachable") (result i32)
    unreachable)
)