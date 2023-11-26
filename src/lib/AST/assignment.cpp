#include "AST/assignment.hpp"

AssignmentNode::AssignmentNode(
        const uint32_t line, const uint32_t col,
        VariableReferenceNode *const p_var_ref,
        ExpressionNode *const p_expr
    ) : AstNode(line, col),
    var_ref(p_var_ref),
    expr(p_expr)
{}

void AssignmentNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    var_ref->accept(p_visitor);
    expr->accept(p_visitor);
}
