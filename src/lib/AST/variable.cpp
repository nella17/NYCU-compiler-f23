#include "AST/variable.hpp"

VariableNode::VariableNode(const uint32_t line,
                           const uint32_t col,
                           char *const p_name,
                           TypePtr p_type,
                           ConstantNodePtr p_constant)
    : AstNode(line, col), name(p_name), type(p_type), constant(p_constant) {
    free(p_name);
}

void VariableNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if (constant)
        constant->accept(p_visitor);
}
