#include "AST/print.hpp"

PrintNode::PrintNode(const uint32_t line,
                     const uint32_t col,
                     ExpressionNode *const p_expr)
    : AstNode(line, col), expr(p_expr) {}

void PrintNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    expr->accept(p_visitor);
}
