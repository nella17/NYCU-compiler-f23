#pragma once

#include "AST/ast.hpp"
#include "AST/CompoundStatement.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <memory>

#include <string>

class ProgramNode final : public AstNode {
  private:
    std::string name;
    // TODO: return type, declarations, functions, compound statement
    std::shared_ptr<CompoundStatementNode> body;

  public:
    ~ProgramNode() = default;
    ProgramNode(const uint32_t line, const uint32_t col,
                const char *const p_name,
                CompoundStatementNode *const p_body
                /* TODO: return type, declarations, functions,
                 *       compound statement */);

    const char *getNameCString() const { return name.c_str(); }
    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
};
