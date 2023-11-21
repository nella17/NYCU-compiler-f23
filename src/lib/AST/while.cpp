#include "AST/while.hpp"

// TODO
WhileNode::WhileNode(const uint32_t line, const uint32_t col)
    : AstNode{line, col} {}

void WhileNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
}
