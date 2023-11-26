#pragma once

#include "AST/ast.hpp"
#include "AST/ConstantValue.hpp"
#include "AST/variable.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <memory>

class DeclNode : public AstNode {
  public:
    // variable declaration
    DeclNode(const uint32_t line, const uint32_t col,
            IDs *const p_ids,
            Type *const p_type);

    // constant variable declaration
    DeclNode(const uint32_t, const uint32_t col,
            IDs *const p_ids,
            ConstantValueNode *const p_constant);

    ~DeclNode() = default;

    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;

    TypePtr getType() const { return type; }
    Variables getVars() const { return vars; }

  private:
    TypePtr type;
    ConstantPtr constant;
    Variables vars;
};

using DeclNodes = std::vector<std::shared_ptr<DeclNode>>;
