#pragma once

#include "AST/CompoundStatement.hpp"
#include "AST/ConstantValue.hpp"
#include "AST/assignment.hpp"
#include "AST/ast.hpp"
#include "AST/decl.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <memory>

class SymbolTable;
using SymbolTablePtr = std::shared_ptr<SymbolTable>;

class ForNode : public AstNode {
  public:
    ForNode(const uint32_t line,
            const uint32_t col,
            DeclNode *const p_decl,
            AssignmentNode *const p_init,
            ConstantValueNode *const p_end,
            CompoundStatementNode *const p_body);
    ~ForNode() = default;

    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
    void visitHeadChildNodes(AstNodeVisitor &p_visitor);
    void visitBodyChildNodes(AstNodeVisitor &p_visitor);

    auto getLoopVarRef() const { return init->getVarRef(); }
    int getBegin() const { return std::get<int>(begin->getValue()); }
    int getEnd() const { return std::get<int>(end->getValue()); }

    auto getSymbolTable() const { return symbol_table; }
    void setSymbolTable(SymbolTablePtr p_symbol_table) {
        symbol_table = p_symbol_table;
    }

  private:
    DeclNodePtr decl;
    AssignmentPtr init;
    ConstantNodePtr begin, end;
    CompoundStatementPtr body;
    SymbolTablePtr symbol_table;
};
