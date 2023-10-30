# WASM API

Build Status: ![build status](https://github.com/gramseyer/wasm_api/actions/workflows/cpp.yml/badge.svg)

This library defines a wrapper around a wasm engine (Wasm3).
This exists, instead of using the wasm engine directly,
as a convenient layer for implementing guards on memory accesses at the wasm sandbox boundary.
This library additionally abstracts away the details of the wasm engine itself.
