#include "AST/decl.hpp"

DeclNode::DeclNode(const uint32_t line,
                   const uint32_t col,
                   IDs *const p_ids,
                   Type *const p_type)
    : AstNode(line, col), type(p_type), constant(nullptr), vars{} {
    init(p_ids);
}

DeclNode::DeclNode(const uint32_t line,
                   const uint32_t col,
                   IDs *const p_ids,
                   ConstantValueNode *const p_constant)
    : AstNode(line, col), type(p_constant->getType()), constant(p_constant),
      vars{} {
    init(p_ids);
}

void DeclNode::init(IDs *const p_ids) {
    vars.reserve(p_ids->size());
    for (auto [id_line, id_col, id_name] : *p_ids)
        vars.emplace_back(std::make_shared<VariableNode>(
            id_line, id_col, id_name, type, constant));
    delete p_ids;
}

void DeclNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    for (auto var : vars)
        var->accept(p_visitor);
}
