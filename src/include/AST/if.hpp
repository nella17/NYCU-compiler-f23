#pragma once

#include "AST/ast.hpp"
#include "AST/statement.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <memory>

class IfNode : public StatementNode {
  public:
    IfNode(const uint32_t line, const uint32_t col
           /* TODO: expression, compound statement, compound statement */);
    ~IfNode() = default;

    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;

  private:
    // TODO: expression, compound statement, compound statement
};
