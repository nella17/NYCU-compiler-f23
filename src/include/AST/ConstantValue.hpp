#pragma once

#include "AST/expression.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <variant>
#include <memory>

class ConstantValueNode : public ExpressionNode {
  public:
    ConstantValueNode(const uint32_t line, const uint32_t col, int p_int);
    ConstantValueNode(const uint32_t line, const uint32_t col, float p_real);
    ConstantValueNode(const uint32_t line, const uint32_t col, char *const p_str);
    ConstantValueNode(const uint32_t line, const uint32_t col, bool p_bool);
    ~ConstantValueNode() = default;

    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }

    std::shared_ptr<Type> getType() const { return type; }

    const char* getNameCString() const { return type->getNameCString(); }
    const char* getValueCString() const { return value_str.c_str(); }

  private:
    std::shared_ptr<Type> type;
    std::variant<int, float, std::string, bool> value;
    std::string value_str;
};
