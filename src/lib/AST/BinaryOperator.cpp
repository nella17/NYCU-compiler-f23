#include "AST/BinaryOperator.hpp"

// TODO
BinaryOperatorNode::BinaryOperatorNode(const uint32_t line, const uint32_t col)
    : AstNode(line, col),
    ExpressionNode(line, col) {}

void BinaryOperatorNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
}
