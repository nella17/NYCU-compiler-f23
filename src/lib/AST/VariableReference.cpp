#include "AST/VariableReference.hpp"

VariableReferenceNode::VariableReferenceNode(
        const uint32_t line, const uint32_t col,
        char *const p_name,
        Expressions *const p_exprs
    ) : ExpressionNode(line, col),
    name(p_name),
    exprs(std::move(*p_exprs))
{
    free(p_name);
    delete p_exprs;
}

void VariableReferenceNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    for (auto expr: exprs)
        expr->accept(p_visitor);
}
