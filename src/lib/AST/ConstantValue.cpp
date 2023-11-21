#include "AST/ConstantValue.hpp"

// TODO
ConstantValueNode::ConstantValueNode(const uint32_t line, const uint32_t col)
    : ExpressionNode{line, col} {}

void ConstantValueNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
}
