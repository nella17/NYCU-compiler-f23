#pragma once

#include "AST/ast.hpp"
#include "AST/decl.hpp"
#include "AST/statement.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <memory>

class CompoundStatementNode : public StatementNode {
  public:
    CompoundStatementNode(const uint32_t line, const uint32_t col,
            DeclNodes *const p_decls,
            Statements *const p_stmts);
    ~CompoundStatementNode() = default;

    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;

  private:
    DeclNodes decls;
    Statements stmts;
};

using CompoundStatementPtr = std::shared_ptr<CompoundStatementNode>;
