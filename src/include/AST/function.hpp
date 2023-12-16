#pragma once

#include "AST/ast.hpp"
#include "AST/decl.hpp"
#include "AST/CompoundStatement.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include "type.hpp"
#include <memory>
#include <vector>

class Args {
    std::string args;
    std::vector<TypePtr> types;
  public:
    Args(const DeclNodes&);
    std::string getPrototypeString() const { return args; }
};
using ArgsPtr = std::shared_ptr<Args>;

class FunctionNode : public AstNode {
  public:
    FunctionNode(const uint32_t line, const uint32_t col,
                char *const p_name,
                DeclNodes *const p_decls,
                Type *const p_type,
                CompoundStatementNode *const p_body);
    ~FunctionNode() = default;

    void accept(AstNodeVisitor &p_visitor) override { p_visitor.visit(*this); }
    void visitChildNodes(AstNodeVisitor &p_visitor) override;

    const char* getNameCString() const { return name.c_str(); }
    std::string getNameString() const { return name; }
    const char* getPrototypeCString() const { return prototype.c_str(); }
    TypePtr getType() const { return type; }
    ArgsPtr getArgs() const { return args; }

  private:
    std::string name, prototype;
    DeclNodes decls;
    ArgsPtr args;
    TypePtr type;
    CompoundStatementPtr body;
};

using Functions = std::vector<std::shared_ptr<FunctionNode>>;
