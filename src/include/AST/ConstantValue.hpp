#pragma once

#include "AST/expression.hpp"
#include "constant.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <memory>

class ConstantValueNode : public ExpressionNode {
  public:
    template <typename T>
    ConstantValueNode(const uint32_t line, const uint32_t col, T value)
        : ExpressionNode(line, col),
          constant(std::make_shared<ConstantValue>(std::forward<T>(value))) {}
    ~ConstantValueNode() = default;

    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }

    const char *getValueCString() const { return constant->getValueCString(); }
    std::string getValueString() const { return constant->getValueString(); }
    auto getValue() const { return constant->getValue(); }
    auto getConstant() const { return constant; }
    auto getType() const { return constant->getType(); }

  private:
    ConstantPtr constant;
};

using ConstantNodePtr = std::shared_ptr<ConstantValueNode>;
