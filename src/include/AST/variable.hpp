#pragma once

#include "AST/ast.hpp"
#include "AST/ConstantValue.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <memory>

class VariableNode : public AstNode {
  public:
    VariableNode(const uint32_t line, const uint32_t col,
            char *const p_name,
            std::shared_ptr<Type> p_type,
            std::shared_ptr<ConstantValueNode> p_constant = nullptr);
    ~VariableNode() = default;

    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;

    const char* getNameCString() const { return name.c_str(); }
    const char* getTypeCString() const { return type->getNameCString(); }

  private:
    std::string name;
    std::shared_ptr<Type> type;
    std::shared_ptr<ConstantValueNode> constant;
};
