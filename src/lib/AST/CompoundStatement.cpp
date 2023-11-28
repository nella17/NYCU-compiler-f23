#include "AST/CompoundStatement.hpp"

CompoundStatementNode::CompoundStatementNode(
    const uint32_t line, const uint32_t col,
    DeclNodes *const p_decls,
    Asts *const p_stmts)
    : AstNode(line, col),
    decls{}, stmts{}
{
    if (p_decls) {
        decls = std::move(*p_decls);
        delete p_decls;
    }
    if (p_stmts) {
        stmts = std::move(*p_stmts);
        delete p_stmts;
    }
}

void CompoundStatementNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    for (auto decl: decls)
        decl->accept(p_visitor);
    for (auto stmt: stmts)
        stmt->accept(p_visitor);
}
