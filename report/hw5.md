# hw5 report

|||
|-:|:-|
|Name|yourname|
|ID|your student id|

## How much time did you spend on this project

* understand homework 2 hr
* implement code generator 10 ~ 14 hr

## Project overview

1. read the TA's implementation of hw4.
2. refactor codes
  * save symbol tables
  * refactor `output_file_path` from filesystem::path
  * set `opt_dmp`
  * define `dumpInstructions` to `fprintf` instead call vfprintf, make compiler analysis arguments type
3. convert test case to c, compile to assembly, implement from assembly
  * `StackManager` to calculate relative offset easily
  * `ebreak` in spike make debug easier (maybe riscv gdb will better?)
  * use `dumpLabel` `branchLabel` `jumpLabel` to implement `if` `while` `for`

## What is the hardest you think in this project

String and boolean bonus is easy.  
But implement floating number is hard :(

## Feedback to T.A.s

Docker image is not arm mac friendly :(

Could TAs release implementation of this homework?  
I want to know how to implement floating-point numbers in a clean way.
