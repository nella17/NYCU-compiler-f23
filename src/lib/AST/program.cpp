#include "AST/program.hpp"

// TODO
ProgramNode::ProgramNode(const uint32_t line, const uint32_t col,
                         const char *const p_name,
                         CompoundStatementNode *const p_body)
    : AstNode{line, col}, name(p_name), body(p_body) {}

void ProgramNode::visitChildNodes(AstNodeVisitor &p_visitor) { // visitor pattern version
    /* TODO
     *
     * for (auto &decl : var_decls) {
     *     decl->accept(p_visitor);
     * }
     *
     * // functions
     */
    body->accept(p_visitor);
}
