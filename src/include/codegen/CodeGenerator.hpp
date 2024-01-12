#pragma once

#include "AST/operator.hpp"
#include "codegen/stack.hpp"
#include "sema/SymbolTable.hpp"
#include "visitor/AstNodeVisitor.hpp"

#include <cstdio>
#include <memory>
#include <string>

class CodeGenerator final : public AstNodeVisitor {
  private:
    SymbolManager m_symbol_manager;
    StackManager m_stack_manager;
    std::string m_source_file_path;
    std::unique_ptr<FILE, decltype(&fclose)> m_output_file{nullptr, &fclose};
    int m_parameter_cnt, m_label_cnt = 0;

    std::string genLabel() { return ".L" + std::to_string(m_label_cnt++); }
    void branchLabel(ExpressionPtr expr, std::string label);
    void branchLabel(Operator op, std::string label);
    void dumpLabel(std::string label);
    void jumpLabel(std::string label);
    void dumpSymbol(std::string name);
    void loadValue(bool copy = true);
    void pusht0();

  public:
    ~CodeGenerator() = default;
    CodeGenerator(const std::string &source_file_name,
                  const std::string &save_path);

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
