#include "AST/function.hpp"

FunctionNode::FunctionNode(const uint32_t line, const uint32_t col,
        char *const p_name,
        DeclNodes *const p_decls,
        Type *const p_ret_type,
        CompoundStatementNode *const p_compound_stmt
    ) : AstNode(line, col),
        name(p_name),
        decls(std::move(*p_decls)),
        ret_type(p_ret_type),
        compound_stmt_p(p_compound_stmt)
    {
    type = ret_type->getNameCString();
    type += " (";
    bool first = true;
    for (auto decl: decls) for (auto var: decl->getVars()) {
        if (first) first = false;
        else type += ", ";
        type += var->getTypeCString();
    }
    type += ')';
    free(p_name);
    delete p_decls;
}

void FunctionNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    for (auto decl: decls)
        decl->accept(p_visitor);
    if (compound_stmt_p)
        compound_stmt_p->accept(p_visitor);
}
