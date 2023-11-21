#ifndef __AST_PRINT_NODE_H
#define __AST_PRINT_NODE_H

#include "AST/ast.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <memory>

class PrintNode : public AstNode {
  public:
    PrintNode(const uint32_t line, const uint32_t col
              /* TODO: expression */);
    ~PrintNode() = default;

    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;

  private:
    // TODO: expression
};

#endif
