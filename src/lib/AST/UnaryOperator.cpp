#include "AST/UnaryOperator.hpp"

// TODO
UnaryOperatorNode::UnaryOperatorNode(const uint32_t line, const uint32_t col)
    : AstNode(line, col),
    ExpressionNode(line, col) {}

void UnaryOperatorNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
}
