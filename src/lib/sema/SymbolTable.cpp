#include "sema/SymbolTable.hpp"

#include <iomanip>

SymbolEntry::SymbolEntry(
    std::string p_name, SymbolKind p_kind, int p_level, TypePtr p_type, AttrT p_attr
): name(p_name), kind(p_kind), level(p_level), type(p_type), attr(p_attr), error(false) {}

SymbolTable::SymbolTable(int p_level): level(p_level) {}

void SymbolTable::addSymbol(SymbolEntryPtr entry) {
    entries.emplace_back(entry);
}

SymbolEntryPtr SymbolManager::addSymbol(
    std::string p_name, SymbolKind p_kind, TypePtr p_type,
    SymbolEntry::AttrT p_attr
) {
    auto prev = lookup(p_name);
    if (prev and (prev->level == level or prev->kind == SymbolKind::kLoopVar)) {
        return nullptr;
    }
    auto entry = std::make_shared<SymbolEntry>(
        p_name, p_kind, level, p_type, p_attr
    );
    currentScope()->addSymbol(entry);
    pushEntry(entry);
    return entry;
}

SymbolEntryPtr SymbolManager::lookup(std::string name) {
    auto it = entries.find(name);
    if (it == entries.end() or it->second.empty()) return nullptr;
    return it->second.back();
}

void SymbolManager::pushEntry(SymbolEntryPtr entry) {
    entries[entry->name].emplace_back(entry);
}

void SymbolManager::popEntry(SymbolEntryPtr entry) {
    auto it = entries.find(entry->name);
    if (it == entries.end() or it->second.empty()) return;
    if (it->second.back() != entry) return;
    it->second.pop_back();
    if (it->second.empty()) entries.erase(it);
}

void SymbolManager::pushGlobalScope() {
    level = -1;
    return pushScope();
}

void SymbolManager::pushScope(SymbolTablePtr table) {
    level++;
    if (!table) table = std::make_shared<SymbolTable>(level);
    tables.emplace_back(table);
    for (auto entry: table->entries)
        pushEntry(entry);
}

void SymbolManager::popScope() {
    auto table = tables.back();
    for (auto entry: table->entries)
        popEntry(entry);
    tables.pop_back();
    level--;
    if (opt_dmp) std::cout << *table;
}

std::ostream& operator<<(std::ostream& os, SymbolKind kind) {
    switch (kind) {
    case SymbolKind::kProgram:
        return os << "program";
    case SymbolKind::kFunction:
        return os << "function";
    case SymbolKind::kParameter:
        return os << "parameter";
    case SymbolKind::kVariable:
        return os << "variable";
    case SymbolKind::kLoopVar:
        return os << "loop_var";
    case SymbolKind::kConstant:
        return os << "constant";
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const SymbolEntry& entry) {
    os << std::left << std::setfill(' ')
        << std::setw(33) << entry.name
        << std::setw(11) << entry.kind
        << std::setw(11) << (std::to_string(entry.level) + (entry.level ? "(local)" : "(global)"))
        << std::setw(17) << entry.type
        << std::setw(11) << entry.attr
        << '\n';
    return os;
}

std::ostream& operator<<(std::ostream& os, const SymbolEntry::AttrT& attr) {
    if (auto constant = std::get_if<ConstantPtr>(&attr); constant and *constant) {
        os << (*constant)->getValueString();
    } else if (auto args = std::get_if<ArgsPtr>(&attr); args and *args) {
        os << (*args)->getPrototypeString();
    }else {
        os << "";
    }
    return os;
}


std::ostream& operator<<(std::ostream& os, const SymbolTable& table) {
    os << std::setfill('=') << std::setw(110) << "" << '\n';
    os << std::left << std::setfill(' ')
        << std::setw(33) << "Name"
        << std::setw(11) << "Kind"
        << std::setw(11) << "Level"
        << std::setw(17) << "Type"
        << std::setw(11) << "Attribute"
        << '\n';
    os << std::setfill('-') << std::setw(110) << "" << '\n';
    for (auto entry: table.entries)
        os << *entry;
    os << std::setfill('-') << std::setw(110) << "" << '\n';
    return os;
}
