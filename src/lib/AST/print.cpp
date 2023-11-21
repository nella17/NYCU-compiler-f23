#include "AST/print.hpp"

// TODO
PrintNode::PrintNode(const uint32_t line, const uint32_t col)
    : AstNode{line, col} {}

void PrintNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
}
