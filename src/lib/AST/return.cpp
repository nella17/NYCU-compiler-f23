#include "AST/return.hpp"

ReturnNode::ReturnNode(const uint32_t line,
                       const uint32_t col,
                       ExpressionNode *const p_expr)
    : AstNode(line, col), expr(p_expr) {}

void ReturnNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    expr->accept(p_visitor);
}
