#include "sema/error.hpp"

std::ostream& operator<<(std::ostream& os, SemanticErrorPtr error) {
    error->dump(os);
    return os;
}

void SemanticError::dump(std::ostream& os) {
    os << "<Error> Found in line " << loc.line << ", column " << loc.col << ": " << reason << '\n'
        << std::string(4, ' ') << lines[loc.line] << '\n'
        << std::string(3 + loc.col, ' ') << "^\n";
}

SemanticError* SymbolRedeclError(Location loc, std::string symbol_name) {
    return new SemanticError(
        loc,
        "symbol '" + symbol_name + "' is redeclared"
    );
}

SemanticError* ArrayDeclGT0Error(Location loc, std::string symbol_name) {
    return new SemanticError(
        loc,
        "'" + symbol_name + "' declared as an array with an index that is not greater than 0"
    );
}

SemanticError* UndeclaredError(Location loc, std::string symbol_name) {
    return new SemanticError(
        loc,
        "use of undeclared symbol '" + symbol_name + "'"
    );
}

SemanticError* NonVariableError(Location loc, std::string symbol_name) {
    return new SemanticError(
        loc,
        "use of non-variable symbol '" + symbol_name + "'"
    );
}

SemanticError* ArrayRefIntError(Location loc) {
    return new SemanticError(
        loc,
        "index of array reference must be an integer"
    );
}

SemanticError* OverArraySubError(Location loc, std::string symbol_name) {
    return new SemanticError(
        loc,
        "there is an over array subscript on '" + symbol_name + "'"
    );
}

SemanticError* InvalidBinaryOp(Location loc, Operator op, TypePtr Ltype, TypePtr Rtype) {
    return new SemanticError(
        loc,
        "invalid operands to binary operator '" + std::string(to_cstring(op)) + "' ('" + Ltype->getNameString() + "' and '" + Rtype->getNameString() + "')"
    );
}

SemanticError* InvalidUnaryOp(Location loc, Operator op, TypePtr type) {
    return new SemanticError(
        loc,
        "invalid operand to unary operator '" + std::string(to_cstring(op)) + "' ('" + type->getNameString() + "')"
    );
}

SemanticError* NonFunctionError(Location loc, std::string symbol_name) {
    return new SemanticError(
        loc,
        "call of non-function symbol '" + symbol_name + "'"
    );
}

SemanticError* ArgsError(Location loc, std::string symbol_name) {
    return new SemanticError(
        loc,
        "too few/much arguments provided for function '" + symbol_name + "'"
    );
}

SemanticError* IncompatibleError(Location loc, TypePtr type_of_argument, TypePtr type_of_parameter) {
    return new SemanticError(
        loc,
        "incompatible type passing '" + type_of_argument->getNameString() + "' to parameter of type '" + type_of_parameter->getNameString() + "'"
    );
}
