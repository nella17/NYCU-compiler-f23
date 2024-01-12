#include "AST/read.hpp"

ReadNode::ReadNode(const uint32_t line,
                   const uint32_t col,
                   VariableReferenceNode *const p_var_ref)
    : AstNode(line, col), var_ref(p_var_ref) {
    var_ref->setAddrUsage();
}

void ReadNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    var_ref->accept(p_visitor);
}
