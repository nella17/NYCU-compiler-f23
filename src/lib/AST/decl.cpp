#include "AST/decl.hpp"

// TODO
DeclNode::DeclNode(const uint32_t line, const uint32_t col)
    : AstNode{line, col} {}

void DeclNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
}
