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
        : value(p_str), value_str(p_str), type(Type::makeString()) {}
    ConstantValue(bool p_bool)
        : value(p_bool), value_str(p_bool ? "true" : "false"),
          type(Type::makeBoolean()) {}

    const char *getValueCString() const { return value_str.c_str(); }
    std::string getValueString() const { return value_str; }
    auto getValue() const { return value; }
    auto getType() const { return type; }

  private:
    Value value;
    std::string value_str;
    TypePtr type;
};

using ConstantPtr = std::shared_ptr<ConstantValue>;
