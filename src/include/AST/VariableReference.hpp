#pragma once

#include "AST/expression.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <memory>
#include <string>

class VariableReferenceNode : public ExpressionNode {
  public:
    VariableReferenceNode(const uint32_t line, const uint32_t col,
            char *const p_name,
            Expressions *const p_exprs);
    ~VariableReferenceNode() = default;

    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;

    const char* getNameCString() const { return name.c_str(); }

  private:
    std::string name;
    Expressions exprs;
};

using VarRefPtr = std::shared_ptr<VariableReferenceNode>;
