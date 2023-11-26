#include "AST/CompoundStatement.hpp"

CompoundStatementNode::CompoundStatementNode(
    const uint32_t line, const uint32_t col,
    DeclNodes *const p_decls,
    StatementNodes *const p_stmts)
    : AstNode(line, col),
    decls(std::move(*p_decls)),
    stmts(std::move(*p_stmts))
    {
    delete p_decls;
    delete p_stmts;
}

void CompoundStatementNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    for (auto decl: decls)
        decl->accept(p_visitor);
    for (auto stmt: stmts)
        stmt->accept(p_visitor);
}
