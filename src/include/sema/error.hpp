#pragma once

#include "AST/ast.hpp"
#include "AST/operator.hpp"
#include "type.hpp"

#include <iostream>
#include <memory>

#define MAX_LINE 200
extern char* lines[MAX_LINE];

class SemanticError {
  protected:
    Location loc;
    std::string reason;

  public:
    SemanticError(Location p_loc, std::string p_reason): loc(p_loc), reason(p_reason) {}
    virtual ~SemanticError() = default;
    void dump(std::ostream&);

    SemanticError(const SemanticError &) = delete;
    SemanticError(SemanticError &&) = delete;
    SemanticError &operator=(const SemanticError &) = delete;
    SemanticError &operator=(SemanticError &&) = delete;
};

using SemanticErrorPtr = std::shared_ptr<SemanticError>;
std::ostream& operator<<(std::ostream& os, SemanticErrorPtr);

SemanticError* SymbolRedeclError(Location, std::string);
SemanticError* ArrayDeclGT0Error(Location, std::string);
SemanticError* UndeclaredError(Location, std::string);
SemanticError* NonVariableError(Location, std::string);
SemanticError* ArrayRefIntError(Location);
SemanticError* OverArraySubError(Location, std::string);
SemanticError* InvalidBinaryOp(Location, Operator, TypePtr, TypePtr);
SemanticError* InvalidUnaryOp(Location, Operator, TypePtr);
SemanticError* NonFunctionError(Location, std::string);
SemanticError* ArgsError(Location, std::string);
SemanticError* IncompatibleParamError(Location, TypePtr, TypePtr);
SemanticError* PrintTypeError(Location);
SemanticError* ReadTypeError(Location);
SemanticError* ReadROError(Location);
SemanticError* ArrayAssignError(Location);
SemanticError* ConstAssignError(Location, std::string);
SemanticError* LoopVarAssignError(Location);
SemanticError* IncompatibleAssignError(Location, TypePtr, TypePtr);
SemanticError* ConditionTypeError(Location);
SemanticError* LoopError(Location);
SemanticError* ReturnVoidError(Location);
SemanticError* ReturnTypeError(Location, TypePtr, TypePtr);
