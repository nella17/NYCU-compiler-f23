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

    const Location &getLocation() const;

    virtual void accept(AstNodeVisitor &p_visitor) = 0;
    virtual void visitChildNodes(AstNodeVisitor &p_visitor){};
};

using IDs = std::vector<std::tuple<uint32_t, uint32_t, char*>>;
using AstPtr = std::shared_ptr<AstNode>;
using Asts = std::vector<AstPtr>;
