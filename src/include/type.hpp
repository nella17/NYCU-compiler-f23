#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>

class Type {
  public:
    static Type *makeVoid();
    static Type *makeInteger();
    static Type *makeReal();
    static Type *makeString();
    static Type *makeBoolean();

    enum class Value {
        Void = 0,
        Integer,
        Real,
        String,
        Boolean,
    };
    const Value value;
    const char *getNameCString();
    std::string getNameString();

    ~Type() = default;

    void addDim(int);
    void popDim();
    bool checkDim();
    bool isVoid() const { return value == Value::Void; }
    bool isInteger() const { return dim.empty() and value == Value::Integer; }
    bool isReal() const { return dim.empty() and value == Value::Real; }
    bool capReal() const { return isReal() or isInteger(); }
    bool isString() const { return dim.empty() and value == Value::String; }
    bool isBool() const { return dim.empty() and value == Value::Boolean; }
    bool isScalar() const { return dim.empty() and value != Value::Void; }
    const auto &getDim() const { return dim; }

    int getAlign() const;
    int getSize() const;

  private:
    Type(Value);
    void ensureName();

    std::vector<int> dim;
    std::string name;
    bool name_valid;
};

using TypePtr = std::shared_ptr<Type>;

bool operator<=(TypePtr, TypePtr);
std::ostream &operator<<(std::ostream &, TypePtr);
