# hw4 report

|||
|-:|:-|
|Name|yourname|
|ID|your student id|

## How much time did you spend on this project

* 3 hours design `SymbolManager`, `SymbolTable` and `SymbolEntry` to pass `1_table`
* 1 hours design `SemanticError` to pass `2_redecl`
* 4 hours implement rest of semantic analysis
* 1 hours refactor
* 1 hours write report

## Project overview

Follow my solution in hw3 to continue working on parser, TA's solution for hw3 delete constructor and assignment operator for move and copy, I copy those part in [60c8bff](https://github.com/compiler-f23/hw4-nella17/commit/60c8bfff3c9248489227bf987746fde84b7f2da1#diff-5769526e49d3c3d0a5b9c87f7e62c260006e7bee4786de9d562ca45d86de6d79).

Implement `SymbolManager`, `SymbolTable` and `SymbolEntry` in single file `include/sema/SymbolTable.hpp` / `lib/sema/SymbolTable.cpp`, follow TA's suggestion in README.  
`SymbolManager` holds `SymbolTable`'s shared pointer in `std::vector`, `pushGlobalScope` in `ProgramNode`, `pushScope` in `FunctionNode`, `CompoundStatementNode` and `ForNode`.  
`SymbolTable` holds `SymbolEntry`'s shared pointer in `std::vector`, maintain hash tables from identifier to `SymbolEntry` using `std::unordered_map` in `SymbolManager`.  
In `SymbolEntry`, store additional attributes' pointer in `std::variant`. save boolean value `error` indicate there are errors in declaration of symbol, to skip some semantic checks.  

Implement `SemanticError` class in `include/sema/error.hpp` / `lib/sema/error.cpp`, inherent `std::exception`.  
Store source code lines begin in `lines_idx`, using `yyin` to read source code.  
Use `try-catch` to show one error related to semantic checks per one AST node, and skip checks when error in declaration.  

Do a little modification add getter function to AST node, some type information to some AST node.  
Add `inferred_type` and `error` to `ExpressionNode` to fufill semantic checks about expressions.  
Do a few refactor, extract `ConstantValue` from `ConstantValueNode`. add `Args` class to represent function arguments type, don't save semantic errors direct output to stderr.

## What is the hardest you think in this project

Design `SymbolManager`, `SymbolTable`, `SymbolEntry` and `SemanticError`.  

## Feedback to T.A.s

Write report harder than write code, I think report should occupy less point, for example (99 coding + 6 report).
