#pragma once

#include "type.hpp"
#include <memory>
#include <string>
#include <variant>

class ConstantValue {
  public:
    using Value = std::variant<int, float, std::string, bool>;
    ConstantValue(int p_int)
        : value(p_int), value_str(std::to_string(p_int)),
          type(Type::makeInteger()) {}
    ConstantValue(float p_real)
        : value(p_real), value_str(std::to_string(p_real)),
          type(Type::makeReal()) {}
    ConstantValue(char *const p_str)
        : value(p_str), value_str(p_str), type(Type::makeString()) {
        free(p_str);
    }
    ConstantValue(bool p_bool)
        : value(p_bool), value_str(p_bool ? "true" : "false"),
          type(Type::makeBoolean()) {}

    const char *getValueCString() const { return value_str.c_str(); }
    std::string getValueString() const { return value_str; }
    auto getValue() const { return value; }
    auto getType() const { return type; }

    auto getIntValue() const { return std::get<int>(value); }
    auto getRealValue() const { return std::get<float>(value); }
    auto getStringValue() const { return std::get<std::string>(value); }
    auto getBooleanValue() const { return std::get<bool>(value); }

    void setLabel(std::string p_label) { label = p_label; }
    auto getLabel() const { return label; }

  private:
    Value value;
    std::string value_str;
    TypePtr type;
    std::string label;
};

using ConstantPtr = std::shared_ptr<ConstantValue>;
