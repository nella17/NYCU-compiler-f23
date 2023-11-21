#pragma once

#include "AST/ast.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <memory>

class VariableNode : public AstNode {
  public:
    VariableNode(const uint32_t line, const uint32_t col
                 /* TODO: variable name, type, constant value */);
    ~VariableNode() = default;

    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;

  private:
    // TODO: variable name, type, constant value
};
