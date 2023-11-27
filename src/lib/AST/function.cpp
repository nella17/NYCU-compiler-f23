#include "AST/function.hpp"

FunctionNode::FunctionNode(const uint32_t line, const uint32_t col,
        char *const p_name,
        DeclNodes *const p_decls,
        Type *const p_type,
        CompoundStatementNode *const p_body
    ) : AstNode(line, col),
        name(p_name),
        decls(std::move(*p_decls)),
        type(p_type),
        body(p_body)
    {
    prototype = type->getNameCString();
    prototype += " (";
    bool first = true;
    for (auto decl: decls) for (auto var: decl->getVars()) {
        if (first) first = false;
        else prototype += ", ";
        prototype += var->getTypeCString();
    }
    prototype += ')';
    free(p_name);
    delete p_decls;
}

void FunctionNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    for (auto decl: decls)
        decl->accept(p_visitor);
    if (body)
        body->accept(p_visitor);
}
