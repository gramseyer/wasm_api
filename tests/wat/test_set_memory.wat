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
  (memory 1 1)

  (func (export "size") (result i64)
    memory.size
    i64.extend_i32_u
  )
  (func (export "load") (param i64) (result i64)
    (i32.wrap_i64 (local.get 0))
    i64.load
  )  
  (func (export "load16") (param i64) (result i64)
    (i64.load16_u (i32.wrap_i64 (local.get 0)))
  )  
  (func (export "load8") (param i64) (result i64)
    (i64.load8_u (i32.wrap_i64 (local.get 0)))
  )
  (func (export "store") (param i64 i64) (result i64)
    (i64.store (i32.wrap_i64 (local.get 0)) (local.get 1))
    (i64.const 0)
  )
  (func (export "store8") (param i64 i64) (result i64)
    (i64.store8 (i32.wrap_i64 (local.get 0)) (local.get 1))
    (i64.const 0)
  )
  (func (export "store16") (param i64 i64) (result i64)
    (i64.store16 (i32.wrap_i64 (local.get 0)) (local.get 1))
    (i64.const 0)
  )
  (export "memory" (memory 0))
)
