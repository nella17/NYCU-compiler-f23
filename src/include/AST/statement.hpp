#pragma once

#include "AST/ast.hpp"
#include <vector>
#include <memory>

class StatementNode : virtual public AstNode {
};

using StatementNodeP = std::shared_ptr<StatementNode>;
using StatementNodes = std::vector<std::shared_ptr<StatementNode>>;
