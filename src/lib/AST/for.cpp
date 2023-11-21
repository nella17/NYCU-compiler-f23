#include "AST/for.hpp"

// TODO
ForNode::ForNode(const uint32_t line, const uint32_t col)
    : AstNode{line, col} {}

void ForNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
}
