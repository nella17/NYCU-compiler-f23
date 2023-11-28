#pragma once

#include "AST/ast.hpp"
#include "AST/assignment.hpp"
#include "AST/decl.hpp"
#include "AST/ConstantValue.hpp"
#include "AST/CompoundStatement.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <memory>

class ForNode : public AstNode {
  public:
    ForNode(const uint32_t line, const uint32_t col,
        DeclNode *const p_decl,
        AssignmentNode *const p_init,
        ConstantValueNode *const p_end,
        CompoundStatementNode *const p_body);
    ~ForNode() = default;

    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;

  private:
    DeclNodePtr decl;
    AssignmentPtr init;
    ConstantPtr end;
    CompoundStatementPtr body;
};
