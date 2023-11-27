#pragma once

#include "AST/ast.hpp"
#include "AST/expression.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <memory>

class PrintNode : public AstNode {
  public:
    PrintNode(const uint32_t line, const uint32_t col,
            ExpressionNode *const p_expr);
    ~PrintNode() = default;

    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;

  private:
    ExpressionPtr expr;
};
