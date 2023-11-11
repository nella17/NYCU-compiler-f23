# hw2 report

|||
|-:|:-|
|Name|yourname|
|ID|your student id|

## How much time did you spend on this project

* read spec 30min
* write grammars 1hr
* write report 30min

## Project overview

Initially, I defined all tokens under `ALL_CASES` use `%token` in `parser.y`, excluding keywords.  
Delimiters, operators, keywords, and literals are each allocated one line.  
Keywords use same format in `LIST_TOKEN`, `KW<lexeme>`.  

Subsequently, I adapted the `scanner.l` file to yield tokens following TA's template for hw1.  
Add return token in each code block following `LIST_TOKEN` or `LIST_LITERAL` macro.

Secondly, I constructed grammar rules according to spec, using `snake_case` to represent non-terminals.  
TA provide the suggestion to write `zero or more something` in two grammars, 
I follow the suggestion for formal arguments and expressions, 
because these two rules require some character between two elements.
In contrast, functions declaration and definition, variable and constant declaration, expressions wrapped by brackets, and statements are implemented within single grammar.

In first time conversion of spec to grammar rules, only 16 / 106 tests are passed.  
It's discovered that there is a misunderstanding the array type, and the array reference allow to use expression between brackets, and some typo in statements rule.
When reading `flex & bison` book, there is an example for negative precedence use `%nonassoc UMINUS`.
This is more appropriate to spec than using same precedence for negative and multiplication.


## What is the hardest you think in this project

Debug the grammars and find how to implement array type and array reference when tests error.

## Feedback to T.A.s

The spec lacked explicit clarification regarding data types, specifically concerning array types.  
Additionally, there was no indication in the spec that array reference could be expression.

Missing `libbison-dev` package in Dockerfile.  
Missing function only test case.
