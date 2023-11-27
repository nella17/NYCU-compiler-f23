#pragma once

#include "AST/ast.hpp"
#include "AST/VariableReference.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <memory>

class ReadNode : public AstNode {
  public:
    ReadNode(const uint32_t line, const uint32_t col,
        VariableReferenceNode *const p_var_ref);
    ~ReadNode() = default;

    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;

  private:
    VarRefPtr var_ref;
};
