#pragma once

#include "AST/ast.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <memory>

class WhileNode : public AstNode {
  public:
    WhileNode(const uint32_t line, const uint32_t col
              /* TODO: expression, compound statement */);
    ~WhileNode() = default;

    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;

  private:
    // TODO: expression, compound statement
};
