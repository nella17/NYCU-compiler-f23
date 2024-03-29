#pragma once

#include "AST/function.hpp"
#include "constant.hpp"
#include "type.hpp"

#include <iostream>
#include <memory>
#include <stack>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

enum class SymbolKind {
    kProgram,
    kFunction,
    kParameter,
    kVariable,
    kLoopVar,
    kConstant,
};

std::ostream &operator<<(std::ostream &, SymbolKind);

class SymbolTable;
class SymbolManager;

class SymbolEntry {
    friend SymbolTable;
    friend SymbolManager;
    friend std::ostream &operator<<(std::ostream &, const SymbolEntry &);

  public:
    using AttrT = std::variant<std::nullptr_t, ConstantPtr, ArgsPtr>;
    SymbolEntry(std::string p_name,
                SymbolKind p_kind,
                int p_level,
                TypePtr p_type,
                AttrT p_attr = nullptr)
        : name(p_name), kind(p_kind), level(p_level), type(p_type),
          attr(p_attr), error(false) {}
    auto getName() const { return name; }
    SymbolKind getKind() const { return kind; }
    int getLevel() const { return level; }
    bool isGlobal() const { return level == 0; }
    void setOffset(size_t p_offset) { offset = p_offset; }
    int getOffset() const { return offset; }
    TypePtr getType() const { return type; }
    bool isError() const { return error; }
    void setError() { error = true; }
    ArgsPtr getArgs() const {
        auto args = std::get_if<ArgsPtr>(&attr);
        return args ? *args : nullptr;
    }

    bool isPointer() const {
        return kind == SymbolKind::kParameter and type->isArray();
    }
    int getSize() const {
        if (isPointer())
            return 4;
        return type->getSize();
    }

  private:
    std::string name;
    SymbolKind kind;
    int level, offset = 0;
    TypePtr type;
    AttrT attr;
    bool error;
};
std::ostream &operator<<(std::ostream &, const SymbolEntry::AttrT &);

using SymbolEntryPtr = std::shared_ptr<SymbolEntry>;
using SymbolEntrys = std::vector<SymbolEntryPtr>;

class SymbolTable {
    friend SymbolManager;
    friend std::ostream &operator<<(std::ostream &, const SymbolTable &);

  public:
    SymbolTable(int p_level) : level(p_level), entries{} {}
    void addSymbol(SymbolEntryPtr);
    // other methods
  private:
    int level;
    SymbolEntrys entries;
};

using SymbolTablePtr = std::shared_ptr<SymbolTable>;
std::ostream &operator<<(std::ostream &, const SymbolTablePtr &);

class SymbolManager {
  private:
    bool opt_dmp;
    int level = 0;
    std::unordered_map<std::string, SymbolEntrys> entries{};
    std::vector<SymbolTablePtr> tables{};
    std::vector<ConstantPtr> constants{};

  public:
    SymbolManager(bool p_opt_dmp) : opt_dmp(p_opt_dmp) {}
    SymbolTablePtr currentScope() { return tables.back(); }
    SymbolEntryPtr addSymbol(std::string,
                             SymbolKind,
                             TypePtr,
                             SymbolEntry::AttrT = nullptr);
    SymbolEntryPtr lookup(std::string);
    void pushEntry(SymbolEntryPtr);
    void popEntry(SymbolEntryPtr);
    void pushGlobalScope();
    void pushScope(SymbolTablePtr = nullptr);
    void popScope();
    void dumpOff() { opt_dmp = false; }
    void pushConstant(ConstantPtr p_constant) {
        constants.emplace_back(p_constant);
    }
    const auto &getConstants() const { return constants; }
};
