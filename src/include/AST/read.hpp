#pragma once

#include "AST/ast.hpp"
#include "AST/statement.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <memory>

class ReadNode : public StatementNode {
  public:
    ReadNode(const uint32_t line, const uint32_t col
             /* TODO: variable reference */);
    ~ReadNode() = default;

    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;

  private:
    // TODO: variable reference
};
