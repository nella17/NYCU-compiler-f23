#include "AST/program.hpp"
#include "AST/AstDumper.hpp"

ProgramNode::ProgramNode(const uint32_t line,
                         const uint32_t col,
                         char *const p_name,
                         DeclNodes *const p_decls,
                         Functions *const p_funcs,
                         CompoundStatementNode *const p_body,
                         Type *const p_type)
    : AstNode(line, col), name(p_name), decls(std::move(*p_decls)),
      funcs(std::move(*p_funcs)), body(p_body), type(p_type) {
    free(p_name);
    delete p_decls;
    delete p_funcs;
}

void ProgramNode::visitChildNodes(
    AstNodeVisitor &p_visitor) { // visitor pattern version
    for (auto decl : decls)
        decl->accept(p_visitor);
    for (auto func : funcs)
        func->accept(p_visitor);
    body->accept(p_visitor);
}
