#pragma once

#include "visitor/AstNodeVisitor.hpp"
#include "sema/SymbolTable.hpp"
#include "sema/error.hpp"

#include <vector>

enum class ContextKind {
    kNone = 0,
    kProgram,
    kFunction,
    kCompound,
    kFor,
};

class SemanticAnalyzer final : public AstNodeVisitor {
  private:
    SymbolManager symbolmanager;
    std::vector<ContextKind> contexts{};
    bool has_error = false;
    std::vector<TypePtr> retTypes;

    bool inFunction() const { return !contexts.empty() and contexts.back() == ContextKind::kFunction; }
    bool inFor() const { return !contexts.empty() and contexts.back() == ContextKind::kFor; }
    SymbolKind varKind(VariableNode&) const;
    void logError(const SemanticError&);

  public:
    ~SemanticAnalyzer() = default;
    SemanticAnalyzer() = default;

    bool hasError() const { return has_error; }

    void visit(ProgramNode &p_program) override;
    void visit(DeclNode &p_decl) override;
    void visit(VariableNode &p_variable) override;
    void visit(ConstantValueNode &p_constant_value) override;
    void visit(FunctionNode &p_function) override;
    void visit(CompoundStatementNode &p_compound_statement) override;
    void visit(PrintNode &p_print) override;
    void visit(BinaryOperatorNode &p_bin_op) override;
    void visit(UnaryOperatorNode &p_un_op) override;
    void visit(FunctionInvocationNode &p_func_invocation) override;
    void visit(VariableReferenceNode &p_variable_ref) override;
    void visit(AssignmentNode &p_assignment) override;
    void visit(ReadNode &p_read) override;
    void visit(IfNode &p_if) override;
    void visit(WhileNode &p_while) override;
    void visit(ForNode &p_for) override;
    void visit(ReturnNode &p_return) override;
};
