#pragma once

#include "sema/SymbolTable.hpp"

#include <cstdio>
#include <vector>

class StackManager {
  private:
    std::vector<int> m_size{}, m_accu{}, m_top{};

  public:
    void push(int p_size, int adj = 8) {
        auto prev = m_accu.empty() ? 0 : m_accu.back() + m_size.back();
        m_size.emplace_back(p_size);
        m_accu.emplace_back(prev);
        m_top.emplace_back(prev + adj);
    }

    int offset(int p_off) { return m_accu.back() - p_off; }
    // TODO
    int offset(int p_off, int p_level) { return m_accu.back() - p_off; }
    int offset(SymbolEntryPtr entry) {
        return offset(entry->getOffset(), entry->getLevel());
    }

    int add(int p_inc) { return m_top.back() += p_inc; }
    int del(int p_inc) { return m_top.back() -= p_inc; }

    void dec(int p_inc) { m_size.back() -= p_inc; }
    void inc(int p_inc) { m_size.back() += p_inc; }

    void pop() {
        m_size.pop_back();
        m_accu.pop_back();
        m_top.pop_back();
    }
};
