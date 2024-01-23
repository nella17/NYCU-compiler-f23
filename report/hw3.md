# hw3 report

|      |                 |
| ---: | :-------------- |
| Name | yourname        |
|   ID | your student id |

## How much time did you spend on this project

* read & understand materials 3 hours
* read TA code & test VP hint 3 hours
* (2) 4 hours
* (3-6) 2 hours
* (7-8,10,11) 1 hours
* (9) 1 hours
* report 1 hours

## Project overview

I follow my implementation of hw2, and references TA's implementation to refine parser.
Such as rename operators, consolidate octal integer into integer literal, convert floating-point number and scientific notation to real literal, rename string to string literal, fix operator precedence, and add `neg_or_empty` and `else_or_empty` terminals.
During modification, I encounter and resolve reduce / reduce conflict (in 4f03217).
Notably, Bison can genreate conflict counterexamples with `-Wcounterexamples` compile option, enhance debug process.

```
$ bison -o parser.cpp --defines=parser.h -v parser.y -Wcounterexamples
parser.y: warning: 2 reduce/reduce conflicts [-Wconflicts-rr]
parser.y: warning: reduce/reduce conflict on tokens INT_LITERAL, REAL_LITERAL [-Wcounterexamples]
  Example: MINUS • INT_LITERAL
  First reduce derivation
    expr
    ↳ 54: literal_constant
          ↳ 81: neg_or_empty  INT_LITERAL
                ↳ 87: MINUS •
  Second reduce derivation
    expr
    ↳ 57: arith_expr
          ↳ 59: MINUS expr
                      ↳ 54: literal_constant
                            ↳ 81: neg_or_empty    INT_LITERAL
                                  ↳ 86: epsilon
                                        ↳ 88: ε •
```

Bison provide `%empty` directive to explicitly define empty rule,
Follow hint.md can pass test-1_program using visitor pattern.
I use smart pointer to handle destructor automatically,
I manage object deletion within constructor, which make me don't need discern which objects are used by AST node and determining the objects requires free in actions.

The most challenging part is test-2_declaration.
I introduce `Type` class for scalar type and array type.
Using `std::vector<std::tuple<uint32_t, uint32_t, char*>>` stores information for `identifier_list` in order to construct `VariableNode` within `DeclNode`.
I use C++ overloads and union to create different type of `ConstantValueNode`.
During the developing, sometimes I want to check rule reduction step, bison can set `yydebug = 1` to enable debug log.

I move AST headers to `%code requires` to eliminate need for manual type many AST nodes forward declarations.
Other code move into `%code top` and `%code`.

Test 3-8 and 10 and 11 follow similar pattern to test2, construct objects, pass to AST node.
Test 9_for is a little interesting, requiring construction of two `ConstantValueNode`, one `DeclNode`, one `AssignmentNode` and one `VariableReferenceNode` within `ForNode`.

### Visitor Pattern

As class diagram below, when AST node accept `AstNodeVisitor`, it invoke coresponding function `AstNodeVisitor::visit` based on its node type.
`AstDumper::visit` dump information about its node, then call `AstNode::visitChildNodes` to recursively dump information about all child nodes.
`AstNode::visitChildNodes` method accept `AstNodeVisitor` for all its child nodes.

<img src="../res/imgs/class_diagram.png" width="800">

## What is the hardest you think in this project

Understand AST and visitor pattern, overcome test2.

## Feedback to T.A.s

Precedence level in hw2 implementation not match hw2 spec.  
Overall, this homework is cool.
