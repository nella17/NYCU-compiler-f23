#pragma once

#include "type.hpp"
#include "AST/ConstantValue.hpp"
#include "AST/function.hpp"

#include <vector>
#include <stack>
#include <memory>
#include <string>
#include <unordered_map>
#include <iostream>
#include <variant>

extern uint32_t opt_dmp;   /* declared in scanner.l */

enum class SymbolKind {
    kProgram,
    kFunction,
    kParameter,
    kVariable,
    kLoopVar,
    kConstant,
};

std::ostream& operator<<(std::ostream&, SymbolKind);

class SymbolTable;
class SymbolManager;

class SymbolEntry {
    friend SymbolTable;
    friend SymbolManager;
    friend std::ostream& operator<<(std::ostream&, const SymbolEntry&);
  public:
    using AttrT = std::variant<std::nullptr_t, ConstantPtr, ArgsPtr>;
    SymbolEntry(std::string, SymbolKind, int, TypePtr, AttrT = nullptr);
    SymbolKind getKind() const { return kind; }
    TypePtr getType() const { return type; }
    bool isError() const { return error; }
    void setError() { error = true; }
  private:
    std::string name;
    SymbolKind kind;
    int level;
    TypePtr type;
    AttrT attr;
    bool error;
};
std::ostream& operator<<(std::ostream&, const SymbolEntry::AttrT&);

using SymbolEntryPtr = std::shared_ptr<SymbolEntry>;
using SymbolEntrys = std::vector<SymbolEntryPtr>;

class SymbolTable {
    friend SymbolManager;
    friend std::ostream& operator<<(std::ostream&, const SymbolTable&);
  public:
    SymbolTable(int);
    void addSymbol(SymbolEntryPtr);
    // other methods
  private:
    int level;
    SymbolEntrys entries;
};

using SymbolTablePtr = std::shared_ptr<SymbolTable>;
std::ostream& operator<<(std::ostream&, const SymbolTablePtr&);

class SymbolManager {
  public:
    SymbolTablePtr currentScope() { return tables.back(); }
    SymbolEntryPtr addSymbol(std::string, SymbolKind, TypePtr, SymbolEntry::AttrT = nullptr);
    SymbolEntryPtr lookup(std::string);
    void pushEntry(SymbolEntryPtr);
    void popEntry(SymbolEntryPtr);
    void pushGlobalScope();
    void pushScope(SymbolTablePtr = nullptr);
    void popScope();
    // other methods
  private:
    int level = 0;
    std::unordered_map<std::string, SymbolEntrys> entries;
    std::vector<SymbolTablePtr> tables;
};
