#include "AST/UnaryOperator.hpp"

UnaryOperatorNode::UnaryOperatorNode(const uint32_t line, const uint32_t col,
        Operator p_op,
        ExpressionNode *const p_expr
    ) : AstNode(line, col),
    ExpressionNode(line, col),
    op(p_op),
    expr(p_expr)
    {}

void UnaryOperatorNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    expr->accept(p_visitor);
}
