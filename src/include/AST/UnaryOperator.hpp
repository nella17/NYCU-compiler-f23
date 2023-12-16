#pragma once

#include "AST/expression.hpp"
#include "AST/operator.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <memory>

class UnaryOperatorNode : public OperatorNode {
  public:
    UnaryOperatorNode(const uint32_t line, const uint32_t col,
            Operator p_op,
            ExpressionNode *const p_expr);
    ~UnaryOperatorNode() = default;

    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;

    ExpressionPtr getExpr() const { return expr; }

  private:
    ExpressionPtr expr;
};
