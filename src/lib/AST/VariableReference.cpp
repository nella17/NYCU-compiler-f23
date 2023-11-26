#include "AST/VariableReference.hpp"

// TODO
VariableReferenceNode::VariableReferenceNode(const uint32_t line,
                                             const uint32_t col)
    : AstNode(line, col),
    ExpressionNode(line, col) {}

void VariableReferenceNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
}
