#include "AST/assignment.hpp"

// TODO
AssignmentNode::AssignmentNode(const uint32_t line, const uint32_t col)
    : AstNode{line, col} {}

void AssignmentNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
}
