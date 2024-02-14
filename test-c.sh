#!/bin/bash
set -ex
riscv32-unknown-elf-gcc -c -S test.c
cat test.s
riscv32-unknown-elf-gcc test.s test/io.c -o test-c.exe
exec spike --isa=RV32GC /risc-v/riscv32-unknown-elf/bin/pk ./test-c.exe
