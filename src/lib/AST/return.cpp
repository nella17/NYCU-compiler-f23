#include "AST/return.hpp"

// TODO
ReturnNode::ReturnNode(const uint32_t line, const uint32_t col)
    : AstNode{line, col} {}

void ReturnNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
}
