#pragma once

#include "sema/SymbolTable.hpp"
#include "sema/error.hpp"
#include "visitor/AstNodeVisitor.hpp"

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
    SymbolManager m_symbol_manager;
    std::vector<ContextKind> m_contexts{};
    bool m_has_error = false;
    std::vector<TypePtr> m_ret_types;

    bool inFunction() const {
        return !m_contexts.empty() and
               m_contexts.back() == ContextKind::kFunction;
    }
    bool inFor() const {
        return !m_contexts.empty() and m_contexts.back() == ContextKind::kFor;
    }
    SymbolKind varKind(VariableNode &) const;
    void logError(const SemanticError &);

  public:
    ~SemanticAnalyzer() = default;
    SemanticAnalyzer(bool opt_dmp) : m_symbol_manager(opt_dmp) {}

    bool hasError() const { return m_has_error; }

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
