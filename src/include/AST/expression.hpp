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
