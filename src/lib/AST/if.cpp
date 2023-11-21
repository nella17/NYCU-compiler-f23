#include "AST/if.hpp"

// TODO
IfNode::IfNode(const uint32_t line, const uint32_t col)
    : AstNode{line, col} {}

void IfNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
}
