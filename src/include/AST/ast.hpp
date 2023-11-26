#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <memory>

class AstNodeVisitor;

struct Location {
    uint32_t line;
    uint32_t col;

    ~Location() = default;
    Location(const uint32_t line, const uint32_t col) : line(line), col(col) {}
};

class AstNode {
  protected:
    Location location;

  public:
    virtual ~AstNode() = 0;
    AstNode(const uint32_t line, const uint32_t col);

    const Location &getLocation() const;

    virtual void accept(AstNodeVisitor &p_visitor) = 0;
    virtual void visitChildNodes(AstNodeVisitor &p_visitor){};
};

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

    Type(Value = Value::Void);
    ~Type() = default;

    void addDim(int);

  private:
    std::vector<int> dim;
    std::string name;
};

using IDs = std::vector<std::tuple<uint32_t, uint32_t, char*>>;
using TypePtr = std::shared_ptr<Type>;
