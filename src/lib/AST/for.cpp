#include "AST/for.hpp"

ForNode::ForNode(
        const uint32_t line, const uint32_t col,
        DeclNode *const p_decl,
        AssignmentNode *const p_init,
        ConstantValueNode *const p_end,
        CompoundStatementNode *const p_body
    ) : AstNode(line, col),
    decl(p_decl), init(p_init), end(p_end), body(p_body)
{}

void ForNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    decl->accept(p_visitor);
    init->accept(p_visitor);
    end->accept(p_visitor);
    body->accept(p_visitor);
}
