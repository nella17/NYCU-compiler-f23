#include "type.hpp"

Type *Type::makeVoid() { return new Type(Value::Void); }
Type *Type::makeInteger() { return new Type(Value::Integer); }
Type *Type::makeReal() { return new Type(Value::Real); }
Type *Type::makeString() { return new Type(Value::String); }
Type *Type::makeBoolean() { return new Type(Value::Boolean); }

Type::Type(Type::Value p_value)
    : value(p_value), dim(0), name(), name_valid(false) {}
void Type::addDim(int x) {
    dim.emplace_back(x);
    name_valid = false;
}
void Type::popDim() {
    if (dim.empty())
        throw std::out_of_range("Type::popDim");
    dim.pop_back();
    name_valid = false;
}

bool Type::checkDim() {
    for (auto d : dim)
        if (d <= 0)
            return false;
    return true;
}

void Type::ensureName() {
    if (!name_valid) {
        switch (value) {
        case Value::Integer:
            name = "integer";
            break;
        case Value::Real:
            name = "real";
            break;
        case Value::String:
            name = "string";
            break;
        case Value::Boolean:
            name = "boolean";
            break;
        case Value::Void:
            name = "void";
            break;
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
}

const char *Type::getNameCString() {
    ensureName();
    return name.c_str();
}

std::string Type::getNameString() {
    ensureName();
    return name;
}

int Type::getAlign() const {
    switch (value) {
    case Value::Integer: {
        return 4;
    }
    default:
        throw "not implemented";
    }
}
int Type::getSize() const {
    if (!dim.empty())
        throw "not implemented";
    switch (value) {
    case Value::Integer: {
        return 4;
    }
    default:
        throw "not implemented";
    }
}

bool operator<=(TypePtr Ltype, TypePtr Rtype) {
    if (Ltype->isInteger() and Rtype->isInteger())
        return true;
    if (Ltype->capReal() and Rtype->isReal())
        return true;
    if (Ltype->getNameString() == Rtype->getNameString())
        return true;
    return false;
}

std::ostream &operator<<(std::ostream &os, TypePtr type) {
    return os << type->getNameString();
}
