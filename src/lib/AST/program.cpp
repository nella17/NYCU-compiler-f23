#include "AST/program.hpp"

#include <iostream>
#include "AST/AstDumper.hpp"

// TODO
ProgramNode::ProgramNode(const uint32_t line, const uint32_t col,
         char *const p_name,
         DeclNodes *const p_decls,
         CompoundStatementNode *const p_body
    ) : AstNode(line, col),
        name(p_name),
        decls(std::move(*p_decls)),
        body(p_body) {
    free(p_name);
    delete p_decls;
}

void ProgramNode::visitChildNodes(AstNodeVisitor &p_visitor) { // visitor pattern version
    for (auto decl : decls)
        decl->accept(p_visitor);
     // functions
    body->accept(p_visitor);
}
