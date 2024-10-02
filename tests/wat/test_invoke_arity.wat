;;
;; Copyright 2023 Geoffrey Ramseyer
;;
;; Licensed under the Apache License, Version 2.0 (the "License");
;; you may not use this file except in compliance with the License.
;; You may obtain a copy of the License at
;;
;;     http://www.apache.org/licenses/LICENSE-2.0
;;
;; Unless required by applicable law or agreed to in writing, software
;; distributed under the License is distributed on an "AS IS" BASIS,
;; WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
;; See the License for the specific language governing permissions and
;; limitations under the License.
;;

(module
  (import "test" "arg0" (func $call0 (result i64)))
  (import "test" "arg1" (func $call1 (param i64) (result i64)))
  (import "test" "arg2" (func $call2 (param i64 i64) (result i64)))
  (import "test" "arg3" (func $call3 (param i64 i64 i64) (result i64)))
  (import "test" "arg4" (func $call4 (param i64 i64 i64 i64) (result i64)))
  (import "test" "arg5" (func $call5 (param i64 i64 i64 i64 i64) (result i64)))
  (import "test" "arg6" (func $call6 (param i64 i64 i64 i64 i64 i64) (result i64)))
  (import "test" "arg7" (func $call7 (param i64 i64 i64 i64 i64 i64 i64) (result i64)))
  (import "test" "arg8" (func $call8 (param i64 i64 i64 i64 i64 i64 i64 i64) (result i64)))

  (import "test" "noret_arg0" (func $noret_call0))
  (import "test" "noret_arg1" (func $noret_call1 (param i64)))
  (import "test" "noret_arg2" (func $noret_call2 (param i64 i64)))
  (import "test" "noret_arg3" (func $noret_call3 (param i64 i64 i64)))
  (import "test" "noret_arg4" (func $noret_call4 (param i64 i64 i64 i64)))
  (import "test" "noret_arg5" (func $noret_call5 (param i64 i64 i64 i64 i64)))
  (import "test" "noret_arg6" (func $noret_call6 (param i64 i64 i64 i64 i64 i64)))
  (import "test" "noret_arg7" (func $noret_call7 (param i64 i64 i64 i64 i64 i64 i64)))
  (import "test" "noret_arg8" (func $noret_call8 (param i64 i64 i64 i64 i64 i64 i64 i64)))

  (func (export "calltest0") (result i64)
    (call $call0)
  )

  (func (export "calltest1") (result i64)
    (call $call1 (i64.const 1))
  )

  (func (export "calltest2") (result i64)
    (call $call2 (i64.const 1) (i64.const 2))
  )

  (func (export "calltest3") (result i64)
    (call $call3 (i64.const 1) (i64.const 2) (i64.const 3))
  )

  (func (export "calltest4") (result i64)
    (call $call4 (i64.const 1) (i64.const 2) (i64.const 3) (i64.const 4))
  )

  (func (export "calltest5") (result i64)
    (call $call5 (i64.const 1) (i64.const 2) (i64.const 3) (i64.const 4) (i64.const 5))
  )

  (func (export "calltest6") (result i64)
    (call $call6 (i64.const 1) (i64.const 2) (i64.const 3) (i64.const 4) (i64.const 5) (i64.const 6))
  )

  (func (export "calltest7") (result i64)
    (call $call7 (i64.const 1) (i64.const 2) (i64.const 3) (i64.const 4) (i64.const 5) (i64.const 6) (i64.const 7))
  )

  (func (export "calltest8") (result i64)
    (call $call8 (i64.const 1) (i64.const 2) (i64.const 3) (i64.const 4) (i64.const 5) (i64.const 6) (i64.const 7) (i64.const 8))
  )

  (func (export "callnoret0") (result i64)
    (call $noret_call0)
    (i64.const 0)
  )

  (func (export "callnoret1") (result i64)
    (call $noret_call1 (i64.const 1))
    (i64.const 0)
  )

  (func (export "callnoret2") (result i64)
    (call $noret_call2 (i64.const 1) (i64.const 2))
    (i64.const 0)
  )

  (func (export "callnoret3") (result i64)
    (call $noret_call3 (i64.const 1) (i64.const 2) (i64.const 3))
    (i64.const 0)
  )

  (func (export "callnoret4") (result i64)
    (call $noret_call4 (i64.const 1) (i64.const 2) (i64.const 3) (i64.const 4))
    (i64.const 0)
  )

  (func (export "callnoret5") (result i64)
    (call $noret_call5 (i64.const 1) (i64.const 2) (i64.const 3) (i64.const 4) (i64.const 5))
    (i64.const 0)
  )

  (func (export "callnoret6") (result i64)
    (call $noret_call6 (i64.const 1) (i64.const 2) (i64.const 3) (i64.const 4) (i64.const 5) (i64.const 6))
    (i64.const 0)
  )
  
  (func (export "callnoret7") (result i64)
    (call $noret_call7 (i64.const 1) (i64.const 2) (i64.const 3) (i64.const 4) (i64.const 5) (i64.const 6) (i64.const 7))
    (i64.const 0)
  )
  
  (func (export "callnoret8") (result i64)
    (call $noret_call8 (i64.const 1) (i64.const 2) (i64.const 3) (i64.const 4) (i64.const 5) (i64.const 6) (i64.const 7) (i64.const 8))
    (i64.const 0)
  )

  (memory 1 1)
  (export "memory" (memory 0))
)
