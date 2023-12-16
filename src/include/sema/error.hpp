#pragma once

#include "AST/ast.hpp"

#include <iostream>
#include <memory>

#define MAX_LINE 200
extern char* lines[MAX_LINE];

class SemanticError {
  protected:
    Location loc;

  public:
    SemanticError(Location p_loc): loc(p_loc) {}
    virtual ~SemanticError() = default;
    virtual void dump(std::ostream&) = 0;
    void show_line_col(std::ostream&);
    void show_source(std::ostream&);

    SemanticError(const SemanticError &) = delete;
    SemanticError(SemanticError &&) = delete;
    SemanticError &operator=(const SemanticError &) = delete;
    SemanticError &operator=(SemanticError &&) = delete;
};

using SemanticErrorPtr = std::shared_ptr<SemanticError>;
std::ostream& operator<<(std::ostream& os, SemanticErrorPtr);

class SymbolRedeclError : public SemanticError {
  private:
    std::string symbol_name;
  public:
    SymbolRedeclError(Location p_loc, std::string p_name): SemanticError(p_loc), symbol_name(p_name) {}
    virtual ~SymbolRedeclError() = default;
    void dump(std::ostream&) override;
};
