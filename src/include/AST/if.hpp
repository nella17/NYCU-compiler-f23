#pragma once

#include "AST/ast.hpp"
#include "AST/expression.hpp"
#include "AST/CompoundStatement.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <memory>

class IfNode : public AstNode {
  public:
    IfNode(const uint32_t line, const uint32_t col,
        ExpressionNode *const p_expr,
        CompoundStatementNode *const p_true_body,
        CompoundStatementNode *const p_false_body);
    ~IfNode() = default;

    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;

  private:
    ExpressionPtr expr;
    CompoundStatementPtr true_body, false_body;
};
