#pragma once

#include "AST/ast.hpp"
#include "AST/decl.hpp"
#include "AST/statement.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <memory>

class CompoundStatementNode : public StatementNode {
  public:
    CompoundStatementNode(const uint32_t line, const uint32_t col
                          /* TODO: declarations, statements */);
    ~CompoundStatementNode() = default;

    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;

  private:
    // TODO: declarations, statements
};

using CompoundStatementNodeP = std::shared_ptr<CompoundStatementNode>;
