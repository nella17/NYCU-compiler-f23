#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <memory>

class AstNodeVisitor;

struct Location {
    uint32_t line;
    uint32_t col;

    ~Location() = default;
    Location(const uint32_t line, const uint32_t col) : line(line), col(col) {}
};

class AstNode {
  protected:
    Location location;

  public:
    virtual ~AstNode() = 0;
    AstNode(const uint32_t line, const uint32_t col);

    // Delete copy/move operations to avoid slicing. [1]
    // And "You almost never want to copy or move polymorphic objects. They
    // generally live on the heap, and are accessed via (smart) pointers." [2]
    // [1]
    // https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rc-copy-virtual
    // [2] https://stackoverflow.com/a/54792149

    AstNode(const AstNode &) = delete;
    AstNode(AstNode &&) = delete;
    AstNode &operator=(const AstNode &) = delete;
    AstNode &operator=(AstNode &&) = delete;

    const Location &getLocation() const;

    virtual void accept(AstNodeVisitor &p_visitor) = 0;
    virtual void visitChildNodes(AstNodeVisitor &p_visitor){};
};

using IDs = std::vector<std::tuple<uint32_t, uint32_t, char*>>;
using AstPtr = std::shared_ptr<AstNode>;
using Asts = std::vector<AstPtr>;
