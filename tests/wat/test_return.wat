;;
;; Copyright 2024 Geoffrey Ramseyer
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
  (type (;0;) (func (result i64)))
  (import "test" "return_fn" (func $ret (type 0)))

  (func (export "returntest") (result i64)
    call $ret
    unreachable
  )
  (memory 1 1)
  (export "memory" (memory 0))
)
