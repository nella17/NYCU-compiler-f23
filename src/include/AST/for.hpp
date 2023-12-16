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

    int getBegin() const { return std::get<int>(begin->getValue()); }
    int getEnd() const { return std::get<int>(end->getValue()); }

  private:
    DeclNodePtr decl;
    AssignmentPtr init;
    ConstantPtr begin, end;
    CompoundStatementPtr body;
};
