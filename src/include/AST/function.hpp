#pragma once

#include "AST/ast.hpp"
#include "AST/decl.hpp"
#include "AST/CompoundStatement.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <memory>
#include <vector>

class FunctionNode : public AstNode {
  public:
    FunctionNode(const uint32_t line, const uint32_t col,
                char *const p_name,
                DeclNodes *const p_decls,
                Type *const p_ret_type,
                CompoundStatementNode *const p_compound_stmt);
    ~FunctionNode() = default;

    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;

    const char* getNameCString() const { return name.c_str(); }
    const char* getTypeCString() const { return type.c_str(); }

  private:
    std::string name, type;
    DeclNodes decls;
    TypeP ret_type;
    CompoundStatementNodeP compound_stmt_p;
};

using FunctionNodes = std::vector<std::shared_ptr<FunctionNode>>;
