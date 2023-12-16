#pragma once

#include "AST/ast.hpp"

#include <iostream>
#include <memory>

#define MAX_LINE 200
extern char* lines[MAX_LINE];

class SemanticError {
  protected:
    Location loc;
    std::string reason;

  public:
    SemanticError(Location p_loc, std::string p_reason): loc(p_loc), reason(p_reason) {}
    virtual ~SemanticError() = default;
    void dump(std::ostream&);

    SemanticError(const SemanticError &) = delete;
    SemanticError(SemanticError &&) = delete;
    SemanticError &operator=(const SemanticError &) = delete;
    SemanticError &operator=(SemanticError &&) = delete;
};

using SemanticErrorPtr = std::shared_ptr<SemanticError>;
std::ostream& operator<<(std::ostream& os, SemanticErrorPtr);

SemanticError* SymbolRedeclError(Location, std::string);
SemanticError* ArrayDeclGT0Error(Location, std::string);
