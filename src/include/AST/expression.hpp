#pragma once

#include "AST/ast.hpp"
#include "type.hpp"
#include <memory>
#include <vector>

class ExpressionNode : public AstNode {
  public:
    ExpressionNode(const uint32_t line, const uint32_t col);
    ~ExpressionNode() = default;

    TypePtr getType() const { return type; }
    void setType(TypePtr p_type) { type = p_type; }
    bool isError() const { return error; }
    void setError() { error = true; }

  protected:
    TypePtr type = nullptr;
    bool error = false;
};

using ExpressionPtr = std::shared_ptr<ExpressionNode>;
using Expressions = std::vector<ExpressionPtr>;
