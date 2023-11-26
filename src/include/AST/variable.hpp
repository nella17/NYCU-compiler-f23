#pragma once

#include "AST/ast.hpp"
#include "AST/ConstantValue.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <memory>
#include <vector>

class VariableNode : public AstNode {
  public:
    VariableNode(const uint32_t line, const uint32_t col,
            char *const p_name,
            TypePtr p_type,
            ConstantPtr p_constant = nullptr);
    ~VariableNode() = default;

    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;

    const char* getNameCString() const { return name.c_str(); }
    const char* getTypeCString() const { return type->getNameCString(); }

  private:
    std::string name;
    TypePtr type;
    ConstantPtr constant;
};

using Variables = std::vector<std::shared_ptr<VariableNode>>;
