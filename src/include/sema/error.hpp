#pragma once

#include "AST/ast.hpp"
#include "AST/operator.hpp"
#include "type.hpp"

#include <iostream>
#include <memory>

#define MAX_LINE 200
#define MAX_LINE_LENG 512
extern uint32_t lines_idx[MAX_LINE];
extern FILE *yyin;

class SemanticError : public std::exception {
  protected:
    const std::string reason;

  public:
    SemanticError(Location p_loc, std::string p_reason)
        : reason(genReason(p_loc, p_reason)) {}
    virtual ~SemanticError() = default;
    const char *what() const noexcept override { return reason.c_str(); }
    static std::string genReason(Location loc, std::string reason);

    SemanticError(const SemanticError &) = delete;
    SemanticError(SemanticError &&) = delete;
    SemanticError &operator=(const SemanticError &) = delete;
    SemanticError &operator=(SemanticError &&) = delete;
};

std::ostream &operator<<(std::ostream &os, const SemanticError &);

class SymbolRedeclError : public SemanticError {
  public:
    SymbolRedeclError(Location loc, std::string symbol_name)
        : SemanticError(loc, "symbol '" + symbol_name + "' is redeclared") {}
};

class ArrayDeclGT0Error : public SemanticError {
  public:
    ArrayDeclGT0Error(Location loc, std::string symbol_name)
        : SemanticError(loc,
                        "'" + symbol_name +
                            "' declared as an array with an index that is "
                            "not greater than 0") {}
};

class UndeclaredError : public SemanticError {
  public:
    UndeclaredError(Location loc, std::string symbol_name)
        : SemanticError(loc, "use of undeclared symbol '" + symbol_name + "'") {
    }
};

class NonVariableError : public SemanticError {
  public:
    NonVariableError(Location loc, std::string symbol_name)
        : SemanticError(loc,
                        "use of non-variable symbol '" + symbol_name + "'") {}
};

class ArrayRefIntError : public SemanticError {
  public:
    ArrayRefIntError(Location loc)
        : SemanticError(loc, "index of array reference must be an integer") {}
};

class OverArraySubError : public SemanticError {
  public:
    OverArraySubError(Location loc, std::string symbol_name)
        : SemanticError(loc,
                        "there is an over array subscript on '" + symbol_name +
                            "'") {}
};

class InvalidBinaryOp : public SemanticError {
  public:
    InvalidBinaryOp(Location loc, Operator op, TypePtr Ltype, TypePtr Rtype)
        : SemanticError(loc,
                        "invalid operands to binary operator '" +
                            std::string(to_cstring(op)) + "' ('" +
                            Ltype->getNameString() + "' and '" +
                            Rtype->getNameString() + "')") {}
};

class InvalidUnaryOp : public SemanticError {
  public:
    InvalidUnaryOp(Location loc, Operator op, TypePtr type)
        : SemanticError(loc,
                        "invalid operand to unary operator '" +
                            std::string(to_cstring(op)) + "' ('" +
                            type->getNameString() + "')") {}
};

class NonFunctionError : public SemanticError {
  public:
    NonFunctionError(Location loc, std::string symbol_name)
        : SemanticError(loc,
                        "call of non-function symbol '" + symbol_name + "'") {}
};

class ArgsError : public SemanticError {
  public:
    ArgsError(Location loc, std::string symbol_name)
        : SemanticError(loc,
                        "too few/much arguments provided for function '" +
                            symbol_name + "'") {}
};

class IncompatibleParamError : public SemanticError {
  public:
    IncompatibleParamError(Location loc,
                           TypePtr type_of_argument,
                           TypePtr type_of_parameter)
        : SemanticError(loc,
                        "incompatible type passing '" +
                            type_of_argument->getNameString() +
                            "' to parameter of type '" +
                            type_of_parameter->getNameString() + "'") {}
};

class PrintTypeError : public SemanticError {
  public:
    PrintTypeError(Location loc)
        : SemanticError(loc,
                        "expression of print statement must be scalar type") {}
};

class ReadTypeError : public SemanticError {
  public:
    ReadTypeError(Location loc)
        : SemanticError(
              loc, "variable reference of read statement must be scalar type") {
    }
};

class ReadROError : public SemanticError {
  public:
    ReadROError(Location loc)
        : SemanticError(loc,
                        "variable reference of read statement cannot be a "
                        "constant or loop variable") {}
};

class ArrayAssignError : public SemanticError {
  public:
    ArrayAssignError(Location loc)
        : SemanticError(loc, "array assignment is not allowed") {}
};

class ConstAssignError : public SemanticError {
  public:
    ConstAssignError(Location loc, std::string symbol_name)
        : SemanticError(loc,
                        "cannot assign to variable '" + symbol_name +
                            "' which is a constant") {}
};

class LoopVarAssignError : public SemanticError {
  public:
    LoopVarAssignError(Location loc)
        : SemanticError(loc,
                        "the value of loop variable cannot be modified "
                        "inside the loop body") {}
};

class IncompatibleAssignError : public SemanticError {
  public:
    IncompatibleAssignError(Location loc,
                            TypePtr type_of_variable_reference,
                            TypePtr type_of_expression)
        : SemanticError(loc,
                        "assigning to '" +
                            type_of_variable_reference->getNameString() +
                            "' from incompatible type '" +
                            type_of_expression->getNameString() + "'") {}
};

class ConditionTypeError : public SemanticError {
  public:
    ConditionTypeError(Location loc)
        : SemanticError(loc,
                        "the expression of condition must be boolean type") {}
};

class LoopError : public SemanticError {
  public:
    LoopError(Location loc)
        : SemanticError(loc,
                        "the lower bound and upper bound of iteration "
                        "count must be in the incremental order") {}
};

class ReturnVoidError : public SemanticError {
  public:
    ReturnVoidError(Location loc)
        : SemanticError(loc, "program/procedure should not return a value") {}
};

class ReturnTypeError : public SemanticError {
  public:
    ReturnTypeError(Location loc,
                    TypePtr type_of_expression,
                    TypePtr return_type)
        : SemanticError(loc,
                        "return '" + type_of_expression->getNameString() +
                            "' from a function with return type '" +
                            return_type->getNameString() + "'") {}
};
