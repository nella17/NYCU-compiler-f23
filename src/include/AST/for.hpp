#pragma once

#include "AST/ast.hpp"
#include "AST/statement.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <memory>

class ForNode : public StatementNode {
  public:
    ForNode(const uint32_t line, const uint32_t col
            /* TODO: declaration, assignment, expression,
             *       compound statement */);
    ~ForNode() = default;

    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;

  private:
    // TODO: declaration, assignment, expression, compound statement
};
