#pragma once

#include "AST/ast.hpp"
#include "AST/ConstantValue.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include "type.hpp"
#include <memory>
#include <vector>

class VariableNode : public AstNode {
  public:
    VariableNode(const uint32_t line, const uint32_t col,
            char *const p_name,
            TypePtr p_type,
            ConstantNodePtr p_constant = nullptr);
    ~VariableNode() = default;

    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;

    const char* getNameCString() const { return name.c_str(); }
    std::string getNameString() const { return name; }
    const char* getTypeCString() const { return type->getNameCString(); }
    std::string getTypeString() const { return type->getNameString(); }
    TypePtr getType() const { return type; }
    ConstantPtr getConstant() const { return constant ? constant->getConstant() : nullptr; }

  private:
    std::string name;
    TypePtr type;
    ConstantNodePtr constant;
};

using VariablePtr = std::shared_ptr<VariableNode>;
using Variables = std::vector<VariablePtr>;
