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
  ;; "add" for direct wasm3 api test
  (func (export "add") (param $lhs i64) (param $rhs i64) (result i64)
    local.get $lhs
    local.get $rhs
    i64.add)

  (func (export "pub01000000") (param $calldata_len i64) (result i64)
    i64.const 0)
)