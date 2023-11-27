#pragma once

#include <memory>
#include <vector>
#include <string>

class Type {
  public:
    static Type* makeVoid();
    static Type* makeInteger();
    static Type* makeReal();
    static Type* makeString();
    static Type* makeBoolean();

    enum class Value {
        Void = 0,
        Integer,
        Real,
        String,
        Boolean,
    };
    const Value value;
    const char* getNameCString();

    ~Type() = default;

    void addDim(int);

  private:
    Type(Value);

    std::vector<int> dim;
    std::string name;
};

using TypePtr = std::shared_ptr<Type>;
