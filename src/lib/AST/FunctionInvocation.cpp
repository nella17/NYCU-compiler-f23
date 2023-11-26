#include "AST/FunctionInvocation.hpp"

// TODO
FunctionInvocationNode::FunctionInvocationNode(const uint32_t line,
                                               const uint32_t col)
    : AstNode(line, col),
    ExpressionNode{line, col} {}

void FunctionInvocationNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
}
