#pragma once

#include "AST/expression.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <string>
#include <memory>

class FunctionInvocationNode : public ExpressionNode {
  public:
    FunctionInvocationNode(const uint32_t line, const uint32_t col,
            char *const p_name,
            Expressions *const p_exprs);
    ~FunctionInvocationNode() = default;

    const char *getNameCString() const { return name.c_str(); }
    std::string getNameString() const { return name; }
    const Expressions& getExprs() const { return exprs; }

    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;

  private:
    std::string name;
    Expressions exprs;
};
