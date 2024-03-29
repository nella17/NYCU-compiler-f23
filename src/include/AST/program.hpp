#pragma once

#include "AST/CompoundStatement.hpp"
#include "AST/ast.hpp"
#include "AST/decl.hpp"
#include "AST/function.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <memory>
#include <string>

class SymbolTable;
using SymbolTablePtr = std::shared_ptr<SymbolTable>;

class ProgramNode final : public AstNode {
  private:
    std::string name;
    DeclNodes decls;
    Functions funcs;
    CompoundStatementPtr body;
    TypePtr type;
    SymbolTablePtr symbol_table;

  public:
    ~ProgramNode() = default;
    ProgramNode(const uint32_t line,
                const uint32_t col,
                char *const p_name,
                DeclNodes *const p_decls,
                Functions *const p_funcs,
                CompoundStatementNode *const p_body,
                Type *const p_type);

    const DeclNodes &getDeclNodes() const { return decls; }
    const Functions &getFuncNodes() const { return funcs; }
    const CompoundStatementNode &getBody() const { return *body.get(); }

    const char *getNameCString() const { return name.c_str(); }
    std::string getNameString() const { return name; }
    TypePtr getType() const { return type; }

    auto getSymbolTable() const { return symbol_table; }
    void setSymbolTable(SymbolTablePtr p_symbol_table) {
        symbol_table = p_symbol_table;
    }

    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
};
