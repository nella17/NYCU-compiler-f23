#pragma once

#include "AST/expression.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <memory>

class BinaryOperatorNode : public ExpressionNode {
  public:
    BinaryOperatorNode(const uint32_t line, const uint32_t col,
            Operator p_op,
            ExpressionNode *const p_left,
            ExpressionNode *const p_right);
    ~BinaryOperatorNode() = default;

    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;

    const char* getOpCString() const { return to_cstring(op); }

  private:
    Operator op;
    ExpressionPtr left, right;
};
