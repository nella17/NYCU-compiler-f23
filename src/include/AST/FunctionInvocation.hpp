#pragma once

#include "AST/expression.hpp"
#include "AST/statement.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <memory>

class FunctionInvocationNode : public StatementNode, public ExpressionNode {
  public:
    FunctionInvocationNode(const uint32_t line, const uint32_t col
                           /* TODO: function name, expressions */);
    ~FunctionInvocationNode() = default;

    const char *getNameCString() const;

    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;

  private:
    // TODO: function name, expressions
};
