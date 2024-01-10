#include "AST/if.hpp"

IfNode::IfNode(const uint32_t line,
               const uint32_t col,
               ExpressionNode *const p_expr,
               CompoundStatementNode *const p_true_body,
               CompoundStatementNode *const p_false_body)
    : AstNode(line, col), expr(p_expr), true_body(p_true_body),
      false_body(p_false_body) {}

void IfNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    expr->accept(p_visitor);
    true_body->accept(p_visitor);
    if (false_body)
        false_body->accept(p_visitor);
}
