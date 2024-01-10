#include "AST/BinaryOperator.hpp"

BinaryOperatorNode::BinaryOperatorNode(const uint32_t line,
                                       const uint32_t col,
                                       Operator p_op,
                                       ExpressionNode *const p_left,
                                       ExpressionNode *const p_right)
    : OperatorNode(line, col, p_op), left(p_left), right(p_right) {}

void BinaryOperatorNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    left->accept(p_visitor);
    right->accept(p_visitor);
}
