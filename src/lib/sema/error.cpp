#include "sema/error.hpp"

std::ostream& operator<<(std::ostream& os, SemanticErrorPtr error) {
    error->show_line_col(os);
    error->dump(os);
    os << '\n';
    error->show_source(os);
    return os;
}

void SemanticError::show_line_col(std::ostream& os) {
    os << "<Error> Found in line " << loc.line << ", column " << loc.col << ": ";
}

void SemanticError::show_source(std::ostream& os) {
    os << std::string(4, ' ') << lines[loc.line] << '\n'
        << std::string(3 + loc.col, ' ') << "^\n";
}

void SymbolRedeclError::dump(std::ostream& os) {
    os << "symbol '" << symbol_name << "' is redeclared";
}
