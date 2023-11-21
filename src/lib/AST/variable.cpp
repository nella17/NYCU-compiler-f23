#include "AST/variable.hpp"

// TODO
VariableNode::VariableNode(const uint32_t line, const uint32_t col)
    : AstNode{line, col} {}

void VariableNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
}
