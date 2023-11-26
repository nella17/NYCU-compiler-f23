#pragma once

#include "AST/ast.hpp"
#include <memory>
#include <vector>

class ExpressionNode : virtual public AstNode {
  public:
    ExpressionNode(const uint32_t line, const uint32_t col);
    ~ExpressionNode() = default;

  protected:
    // for carrying type of result of an expression
    // TODO: for next assignment
};

using ExpressionPtr = std::shared_ptr<ExpressionNode>;
using Expressions = std::vector<ExpressionPtr>;

enum class Operator {
    MUL, DIV, MOD, ADD, SUB,
    OP_LT, OP_LTEQ, OP_NEQ, OP_GTEQ, OP_GT, OP_EQ,
    AND, OR,
    NEG, NOT
};

const char* to_cstring(Operator op);
