# ByteBuffer

ByteBuffer Implementation in x86_64 Assembly Language with C Interface.

By JD McIntosh

[![License: GPL-2.0](https://img.shields.io/badge/License-GPL%202.0-blue.svg)](https://opensource.org/licenses/GPL-2.0)
[![Stars](https://img.shields.io/github/stars/KatoKode/ByteBuffer?style=social)](https://github.com/KatoKode/ByteBuffer/stargazers)

## Description

This project provides an efficient implementation of a ByteBuffer, inspired by similar constructs in languages like Java. The core functionality is written in x86_64 assembly language for optimal performance, while exposing a clean C interface for integration into applications. The ByteBuffer is compiled as a shared library (`libbytebuffer.so`), making it easy to link against in C programs. It includes support for reading and writing various data types at the byte level, buffer position management, and utility functions.

The repository also includes a utility library (`libutil.so`) with an optimized routine (e.g., a custom `memmove` for non-overlapping copies) and a demo program to illustrate usage.

## Features

- **Assembly-Optimized Core**: Key operations implemented in x86_64 assembly for high performance on Linux systems.
- **C Interface**: Simple and intuitive API for buffer management, compatible with C programs.
- **Data Type Support**: Methods to put and get:
  - Bytes (`byte_t`)
  - 16-bit integers (`int16_t` and `uint16_t`)
  - 32-bit integers (`int32_t` and `uint32_t`)
  - 64-bit integers (`int64_t` and `uint64_t`)
  - Floating-point numbers (`float`)
  - Double-precision floating-point numbers (`double`)
  - Variable-length strings (`varchar`)
- **Buffer Operations**:
  - Initialization and termination (`bb_init`, `bb_term`)
  - Clearing the buffer (`bb_clear`)
  - Flipping for read/write mode switching (`bb_flip`)
  - Rewinding to the start (`bb_rewind`)
  - Marking and resetting positions (`bb_mark`, `bb_reset`)
- **Position and Bounds Management**: Track and query index, bound, remaining bytes, and size (`bb_get_index`, `bb_get_bound`, `bb_get_remaining`, `bb_get_size`).
- **Endianness Detection**: Check native byte order (`bb_native_byte_order`).
- **Indexed Access**: Put and get values at specific indices (`bb_put_at`, `bb_get_at`, etc.).
- **Utility Function**: Includes optimized memory operation in `memmove64` for efficient data copying.
- **Debug Support**: Optional debug output via `#define BB_DEBUG` in demo program.
- **Demo Program**: A C-based example demonstrating buffer usage, including putting values, flipping, and getting values.

## Requirements

- Linux OS
- Programming languages: Assembly (x86_64), C
- Tools: Netwide Assembler (NASM), GCC compiler, and the `make` utility
- Text editor and working at the command-line

## Installation

1. Clone the repository:
```bash
   git clone https://github.com/KatoKode/ByteBuffer.git
```

## Build
Run the following command in the `ByteBuffer-main` folder:
```bash
sh ./bytebuffer_make.sh
```

## Run
In the `demo` folder enter the following command:
```bash
./go_demo.sh
```

## Have Fun!
