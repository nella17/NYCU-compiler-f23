#pragma once

#include "AST/ast.hpp"
#include "AST/decl.hpp"
#include "AST/function.hpp"
#include "AST/CompoundStatement.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <memory>
#include <string>

class ProgramNode final : public AstNode {
  private:
    std::string name;
    DeclNodes decls;
    FunctionNodes funcs;
    // TODO: return type
    CompoundStatementNodeP body;

  public:
    ~ProgramNode() = default;
    ProgramNode(const uint32_t line, const uint32_t col,
                char *const p_name,
                DeclNodes *const p_decls,
                FunctionNodes *const p_funcs,
                CompoundStatementNode *const p_body
                /* TODO: return type */);

    const char *getNameCString() const { return name.c_str(); }
    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
};
