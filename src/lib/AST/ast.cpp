#include <AST/ast.hpp>

// prevent the linker from complaining
AstNode::~AstNode() {}

AstNode::AstNode(const uint32_t line, const uint32_t col)
    : location(line, col) {}

const Location &AstNode::getLocation() const { return location; }

Type* Type::makeVoid()    { return new Type(Value::Void); }
Type* Type::makeInteger() { return new Type(Value::Integer); }
Type* Type::makeReal()    { return new Type(Value::Real); }
Type* Type::makeString()  { return new Type(Value::String); }
Type* Type::makeBoolean() { return new Type(Value::Boolean); }

Type::Type(Type::Value p_value): value(p_value), dim(0), name() {}
void Type::addDim(int x) { dim.emplace_back(x); }

const char* Type::getNameCString() {
    switch (value) {
        case Value::Integer: name = "integer"; break;
        case Value::Real   : name = "real"; break;
        case Value::String : name = "string"; break;
        case Value::Boolean: name = "boolean"; break;
        case Value::Void   : name = "void"; break;
    }
    if (!dim.empty()) {
        name += ' ';
        for (auto it = dim.rbegin(); it != dim.rend(); it++) {
            name += '[';
            name += std::to_string(*it);
            name += ']';
        }
    }
    return name.c_str();
}
