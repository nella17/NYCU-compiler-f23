#ifndef __AST_FOR_NODE_H
#define __AST_FOR_NODE_H

#include "AST/ast.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <memory>

class ForNode : public AstNode {
  public:
    ForNode(const uint32_t line, const uint32_t col
            /* TODO: declaration, assignment, expression,
             *       compound statement */);
    ~ForNode() = default;

    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;

  private:
    // TODO: declaration, assignment, expression, compound statement
};

#endif
