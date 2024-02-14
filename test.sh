#!/bin/bash
set -ex
DEBUG=1 make -j$(nproc) project
./src/compiler test.p --dump-ast
cat test.S
riscv32-unknown-elf-gcc test.S test/io.c -o test.exe
exec spike --isa=RV32GC /risc-v/riscv32-unknown-elf/bin/pk ./test.exe
