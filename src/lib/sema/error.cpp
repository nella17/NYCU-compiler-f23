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
