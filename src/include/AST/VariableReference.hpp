#pragma once

#include "AST/expression.hpp"
#include "AST/variable.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <memory>
#include <string>

class VariableReferenceNode : public ExpressionNode {
  public:
    VariableReferenceNode(const uint32_t line,
                          const uint32_t col,
                          char *const p_name,
                          Expressions *const p_exprs);
    VariableReferenceNode(const uint32_t line,
                          const uint32_t col,
                          VariablePtr const p_ptr,
                          Expressions *const p_exprs);
    ~VariableReferenceNode() = default;

    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;

    const char *getNameCString() const { return name.c_str(); }
    std::string getNameString() const { return name; }
    const Expressions &getExprs() const { return exprs; }

    void setAddrUsage() { addr_usage = true; }
    bool isValueUsage() const { return !addr_usage; }

  private:
    bool addr_usage = false;
    std::string name;
    Expressions exprs;
};

using VarRefPtr = std::shared_ptr<VariableReferenceNode>;
