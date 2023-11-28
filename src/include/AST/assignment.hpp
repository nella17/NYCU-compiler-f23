#pragma once

#include "AST/ast.hpp"
#include "AST/VariableReference.hpp"
#include "AST/expression.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <memory>

class AssignmentNode : public AstNode {
  public:
    AssignmentNode(const uint32_t line, const uint32_t col,
        VariableReferenceNode *const p_var_ref,
        ExpressionNode *const p_expr);
    ~AssignmentNode() = default;

    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;

  private:
    VarRefPtr var_ref;
    ExpressionPtr expr;
};

using AssignmentPtr = std::shared_ptr<AssignmentNode>;
