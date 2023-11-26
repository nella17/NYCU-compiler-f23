#pragma once

#include "AST/expression.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <memory>

class UnaryOperatorNode : public ExpressionNode {
  public:
    UnaryOperatorNode(const uint32_t line, const uint32_t col,
            Operator p_op,
            ExpressionNode *const p_expr);
    ~UnaryOperatorNode() = default;

    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;

    const char* getOpCString() const { return to_cstring(op); }

  private:
    Operator op;
    ExpressionPtr expr;
};
