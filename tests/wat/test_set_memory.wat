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

  (func (export "size") (result i32) (memory.size))
  (func (export "load") (param i32) (result i32)
    (i32.load (local.get 0))
  )  
  (func (export "load16") (param i32) (result i32)
    (i32.load16_u (local.get 0))
  )  
  (func (export "load8") (param i32) (result i32)
    (i32.load8_u (local.get 0))
  )
  (func (export "store") (param i32 i32)
    (i32.store (local.get 0) (local.get 1))
  )
  (func (export "store8") (param i32 i32)
    (i32.store8 (local.get 0) (local.get 1))
  )
  (func (export "store16") (param i32 i32)
    (i32.store16 (local.get 0) (local.get 1))
  )
  (export "memory" (memory 0))
)
