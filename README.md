# Utility CPP

A library written in C++17 containing various utilities.

# Modules
- upp::cli -- Command Line Interface
  - `upp::cli` -- Hierarchical command line parser
- upp::sync -- Various helpers for interthread communication
  - `upp::sync::Fifo` -- Threadsafe Fifo buffer based on synchronization mechanisms in standard library
  - `upp::sync::MutexWrap` -- Rust-like wrapper class providing thread safe access to the wrapped value
- upp::parser -- Lexer/parser library supporting Backus-Naur form-like grammar specification

# Examples
- TBA

# Requirements
General:
- C++17
- g++
  - Verified to work with g++ 7.4.0
- cmake 3.10

Unit tests
- googletest
- Optional: gcovr (needed for coverage reports)

# Directory Structure
- include/upp -- Contains header files
- samples -- Examples for using the library
- src -- Contains library source files
- unit-test -- Contains unit tests
