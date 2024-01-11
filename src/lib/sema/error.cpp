#include "sema/error.hpp"

#include <cstring>

std::string getSource(uint32_t line) {
    size_t size = lines_idx[line] - lines_idx[line - 1];
    char buf[size];
    if (fseek(yyin, lines_idx[line - 1], SEEK_SET) < 0 or
        fgets(buf, (int)size, yyin) == NULL)
        return std::strerror(errno);
    return {buf, size - 1};
}

std::string SemanticError::genReason(Location loc, std::string reason) {
    // clang-format off
    return "<Error> Found in line " + std::to_string(loc.line)
        + ", column " + std::to_string(loc.col) + ": " + reason + '\n'
        + "    " + getSource(loc.line) + '\n'
        + std::string(3 + loc.col, ' ') + "^\n";
    // clang-format on
}
