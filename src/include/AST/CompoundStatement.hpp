#pragma once

#include "AST/ast.hpp"
#include "AST/decl.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <memory>

class SymbolTable;
using SymbolTablePtr = std::shared_ptr<SymbolTable>;

class CompoundStatementNode : public AstNode {
  public:
    CompoundStatementNode(const uint32_t line,
                          const uint32_t col,
                          DeclNodes *const p_decls,
                          Asts *const p_stmts);
    ~CompoundStatementNode() = default;

    auto getSymbolTable() const { return symbol_table; }
    void setSymbolTable(SymbolTablePtr p_symbol_table) {
        symbol_table = p_symbol_table;
    }

    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;

  private:
    DeclNodes decls;
    Asts stmts;
    SymbolTablePtr symbol_table;
};

using CompoundStatementPtr = std::shared_ptr<CompoundStatementNode>;
