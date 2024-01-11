#include "codegen/CodeGenerator.hpp"
#include "visitor/AstNodeInclude.hpp"

#include <algorithm>
#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <filesystem>
namespace fs = std::filesystem;

CodeGenerator::CodeGenerator(const std::string &source_file_path,
                             const std::string &save_path)
    : symbol_manager(false), m_source_file_path(source_file_path) {
    // FIXME: assume that the source file is always xxxx.p
    fs::path real_path = save_path.empty() ? std::string{"."} : save_path;
    fs::path source_path = source_file_path;
    source_path.replace_extension(".S");
    fs::path output_file_path = real_path / source_path.filename();
    auto file = fopen(output_file_path.c_str(), "w");
    assert(file && "Failed to open output file");
    m_output_file.reset(file);
}

static void dumpInstructions(FILE *p_out_file, const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(p_out_file, format, args);
    va_end(args);
}

void CodeGenerator::visit(ProgramNode &p_program) {
    // Generate RISC-V instructions for program header
    // clang-format off
    constexpr const char *const riscv_assembly_file_prologue =
        "    .file \"%s\"\n"
        "    .option nopic\n"
        ;
    // clang-format on
    dumpInstructions(m_output_file.get(), riscv_assembly_file_prologue,
                     m_source_file_path.c_str());

    symbol_manager.pushScope(p_program.getSymbolTable());

    auto visit_ast_node = [&](auto &ast_node) { ast_node->accept(*this); };
    for_each(p_program.getDeclNodes().begin(), p_program.getDeclNodes().end(),
             visit_ast_node);
    for_each(p_program.getFuncNodes().begin(), p_program.getFuncNodes().end(),
             visit_ast_node);

    // clang-format off
    constexpr const char *const riscv_assembly_main_prologue =
        ".section    .text\n"
        "    .align 2\n"
        "    .global main\n"
        "    .type main, @function\n"
        "main:\n"
        "    addi sp, sp, -12\n"
        "    sw ra, 124(sp)\n"
        "    sw s0, 120(sp)\n"
        "    addi s0, sp, 128\n"
        ;
    // clang-format on
    dumpInstructions(m_output_file.get(), riscv_assembly_main_prologue);

    const_cast<CompoundStatementNode &>(p_program.getBody()).accept(*this);

    // clang-format off
    constexpr const char *const riscv_assembly_main_epilogue =
        "    lw ra, 124(sp)\n"
        "    lw s0, 120(sp)\n"
        "    addi sp, sp, 128\n"
        "    jr ra\n"
        "    .size main, .-main\n"
        ;
    // clang-format on
    dumpInstructions(m_output_file.get(), riscv_assembly_main_epilogue);

    symbol_manager.popScope();
}

void CodeGenerator::visit(DeclNode &p_decl) {}

void CodeGenerator::visit(VariableNode &p_variable) {}

void CodeGenerator::visit(ConstantValueNode &p_constant_value) {}

void CodeGenerator::visit(FunctionNode &p_function) {
    symbol_manager.pushScope(p_function.getSymbolTable());

    p_function.visitChildNodes(*this);

    symbol_manager.popScope();
}

void CodeGenerator::visit(CompoundStatementNode &p_compound_statement) {
    symbol_manager.pushScope(p_compound_statement.getSymbolTable());

    p_compound_statement.visitChildNodes(*this);

    symbol_manager.popScope();
}

void CodeGenerator::visit(PrintNode &p_print) {}

void CodeGenerator::visit(BinaryOperatorNode &p_bin_op) {}

void CodeGenerator::visit(UnaryOperatorNode &p_un_op) {}

void CodeGenerator::visit(FunctionInvocationNode &p_func_invocation) {}

void CodeGenerator::visit(VariableReferenceNode &p_variable_ref) {}

void CodeGenerator::visit(AssignmentNode &p_assignment) {}

void CodeGenerator::visit(ReadNode &p_read) {}

void CodeGenerator::visit(IfNode &p_if) {}

void CodeGenerator::visit(WhileNode &p_while) {}

void CodeGenerator::visit(ForNode &p_for) {
    symbol_manager.pushScope(p_for.getSymbolTable());

    p_for.visitChildNodes(*this);

    symbol_manager.popScope();
}

void CodeGenerator::visit(ReturnNode &p_return) {}
