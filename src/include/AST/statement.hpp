#pragma once

#include "AST/ast.hpp"
#include <vector>
#include <memory>

class StatementNode : virtual public AstNode {
};

using StatementPtr = std::shared_ptr<StatementNode>;
using Statements = std::vector<std::shared_ptr<StatementNode>>;
