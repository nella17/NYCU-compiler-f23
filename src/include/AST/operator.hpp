#pragma once

#include "AST/expression.hpp"

enum class Operator {
    MUL, DIV, MOD, ADD, SUB,
    OP_LT, OP_LTEQ, OP_NEQ, OP_GTEQ, OP_GT, OP_EQ,
    AND, OR,
    NEG, NOT
};

const char* to_cstring(Operator op);

class OperatorNode : public ExpressionNode {
  public:
    OperatorNode(const uint32_t line, const uint32_t col, Operator p_op);
    ~OperatorNode() = default;

    const char* getOpCString() const { return to_cstring(op); }
    Operator getOp() const { return op; }

  protected:
    const Operator op;
};
