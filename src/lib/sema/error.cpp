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
