#pragma once

#include "AST/ast.hpp"
#include "AST/expression.hpp"
#include "AST/CompoundStatement.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <memory>

class WhileNode : public AstNode {
  public:
    WhileNode(const uint32_t line, const uint32_t col,
        ExpressionNode *const p_expr,
        CompoundStatementNode *const p_body);
    ~WhileNode() = default;

    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;

  private:
    ExpressionPtr expr;
    CompoundStatementPtr body;
};
