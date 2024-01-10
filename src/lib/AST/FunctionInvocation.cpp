#include "AST/FunctionInvocation.hpp"

FunctionInvocationNode::FunctionInvocationNode(const uint32_t line,
                                               const uint32_t col,
                                               char *const p_name,
                                               Expressions *const p_exprs)
    : ExpressionNode(line, col), name(p_name), exprs{} {
    free(p_name);
    if (p_exprs) {
        exprs = std::move(*p_exprs);
        delete p_exprs;
    }
}

void FunctionInvocationNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    for (auto expr : exprs)
        expr->accept(p_visitor);
}
