#include "type.hpp"

Type* Type::makeVoid()    { return new Type(Value::Void); }
Type* Type::makeInteger() { return new Type(Value::Integer); }
Type* Type::makeReal()    { return new Type(Value::Real); }
Type* Type::makeString()  { return new Type(Value::String); }
Type* Type::makeBoolean() { return new Type(Value::Boolean); }

Type::Type(Type::Value p_value): value(p_value), dim(0), name(), name_valid(false) {}
void Type::addDim(int x) {
    dim.emplace_back(x);
    name_valid = false;
}

const char* Type::getNameCString() {
    if (!name_valid) {
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
        name_valid = true;
    }
    return name.c_str();
}
