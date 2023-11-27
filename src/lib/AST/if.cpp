#include "AST/if.hpp"

IfNode::IfNode(
        const uint32_t line, const uint32_t col,
        ExpressionNode *const p_expr,
        CompoundStatementNode *const p_true_stmt,
        CompoundStatementNode *const p_false_stmt
    ) : AstNode(line, col),
    expr(p_expr),
    true_stmt(p_true_stmt),
    false_stmt(p_false_stmt)
{}

void IfNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    expr->accept(p_visitor);
    true_stmt->accept(p_visitor);
    if (false_stmt)
        false_stmt->accept(p_visitor);
}
