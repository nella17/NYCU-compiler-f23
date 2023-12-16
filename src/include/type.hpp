#pragma once

#include <memory>
#include <vector>
#include <string>
#include <iostream>

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
    std::string getNameString();

    ~Type() = default;

    void addDim(int);
    bool checkDim();
    bool isInteger();
    size_t getSize() const { return dim.size(); }

  private:
    Type(Value);
    void ensureName();

    std::vector<int> dim;
    std::string name;
    bool name_valid;
};

using TypePtr = std::shared_ptr<Type>;
std::ostream& operator<<(std::ostream&, TypePtr);
