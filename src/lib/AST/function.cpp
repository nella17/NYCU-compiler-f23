#include "AST/function.hpp"

Args::Args(const DeclNodes& p_decls): args{}, types{} {
    bool first = true;
    for (auto decl: p_decls) for (auto var: decl->getVars()) {
        if (first) first = false;
        else args += ", ";
        args += var->getTypeString();
        types.emplace_back(var->getType());
    }
}

FunctionNode::FunctionNode(const uint32_t line, const uint32_t col,
        char *const p_name,
        DeclNodes *const p_decls,
        Type *const p_type,
        CompoundStatementNode *const p_body
    ) : AstNode(line, col),
        name(p_name),
        decls(std::move(*p_decls)),
        args(new Args(decls)),
        type(p_type),
        body(p_body)
{
    free(p_name);
    delete p_decls;
    prototype = type->getNameString() + " (" + args->getPrototypeString() + ")";
}

void FunctionNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    for (auto decl: decls)
        decl->accept(p_visitor);
    if (body)
        body->accept(p_visitor);
}
