#include "AST/while.hpp"

WhileNode::WhileNode(
        const uint32_t line, const uint32_t col,
        ExpressionNode *const p_expr,
        CompoundStatementNode *const p_body
    ) : AstNode(line, col),
    expr(p_expr), body(p_body)
{}

void WhileNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    expr->accept(p_visitor);
    body->accept(p_visitor);
}
