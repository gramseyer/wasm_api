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
  (type (;0;) (func (param i32) (result i32)))
  (import "test" "redir_call" (func $foo (type 0)))

  (func (export "calltest") (result i32)
    i32.const 12
    call $foo
  ) 
  (memory 1 1)
)
