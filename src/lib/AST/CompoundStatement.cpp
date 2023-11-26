#include "AST/CompoundStatement.hpp"

// TODO
CompoundStatementNode::CompoundStatementNode(const uint32_t line,
                                             const uint32_t col)
    : AstNode(line, col) {}

void CompoundStatementNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
}
