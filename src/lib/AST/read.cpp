#include "AST/read.hpp"

// TODO
ReadNode::ReadNode(const uint32_t line, const uint32_t col)
    : AstNode{line, col} {}

void ReadNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
}
