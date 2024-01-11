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
    : m_symbol_manager(false), m_source_file_path(source_file_path) {
    // FIXME: assume that the source file is always xxxx.p
    fs::path real_path = save_path.empty() ? std::string{"."} : save_path;
    fs::path source_path = source_file_path;
    source_path.replace_extension(".S");
    fs::path output_file_path = real_path / source_path.filename();
    auto file = fopen(output_file_path.c_str(), "w");
    assert(file && "Failed to open output file");
    m_output_file.reset(file);
}

#define dumpInstructions(p_out_file, format, ...)                              \
    fprintf(p_out_file, format __VA_OPT__(, ) __VA_ARGS__)

#define riscvAsmPush(reg)                                                      \
    "    addi sp, sp, -4\n"                                                    \
    "    sw " #reg ", 0(sp)\n"
#define riscvAsmPop(reg)                                                       \
    "    lw " #reg ", 0(sp)\n"                                                 \
    "    addi sp, sp, 4\n"

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

    m_symbol_manager.pushScope(p_program.getSymbolTable());

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
        "  # prelogue \n"
        "    addi sp, sp, -128\n"
        "    sw ra, 124(sp)\n"
        "    sw s0, 120(sp)\n"
        "    addi s0, sp, 128\n"
        ;
    // clang-format on
    dumpInstructions(m_output_file.get(), riscv_assembly_main_prologue);
    m_stack_manager.push(128);

    const_cast<CompoundStatementNode &>(p_program.getBody()).accept(*this);

    // clang-format off
    constexpr const char *const riscv_assembly_main_epilogue =
        "  # epilogue \n"
        "    lw ra, 124(sp)\n"
        "    lw s0, 120(sp)\n"
        "    addi sp, sp, 128\n"
        "    jr ra\n"
        "    .size main, .-main\n"
        ;
    // clang-format on
    dumpInstructions(m_output_file.get(), riscv_assembly_main_epilogue);
    m_stack_manager.pop();

    m_symbol_manager.popScope();
}

void CodeGenerator::visit(DeclNode &p_decl) { p_decl.visitChildNodes(*this); }

void CodeGenerator::visit(VariableNode &p_variable) {
    auto name = p_variable.getNameString();
    auto type = p_variable.getType();
    auto constant = p_variable.getConstant();
    auto entry = m_symbol_manager.lookup(name);
    auto align = type->getAlign();
    auto size = type->getSize();
    if (entry->isGlobal()) {
        // clang-format off
        constexpr const char *const riscv_assembly_global_int =
            "    .global %s\n"
            "    .section %s\n"
            "    .align %d\n"
            "    .type %s, @object\n"
            "    .size %s, %d\n"
            "%s:\n"
            "    .word %d\n"
            ;
        // clang-format on
        const char *section = constant ? ".rodata" : ".bss";
        auto name_str = name.c_str();
        switch (type->value) {
        case Type::Value::Integer: {
            int value = constant ? constant->getIntValue() : 0;
            dumpInstructions(m_output_file.get(), riscv_assembly_global_int,
                             name_str, section, align, name_str, name_str, size,
                             name_str, value);
            break;
        }
        default:
            throw "not implemented";
        }
    } else {
        auto offset = m_stack_manager.add(size);
        entry->setOffset(offset);
        if (constant) {
            p_variable.visitChildNodes(*this);
            dumpInstructions(m_output_file.get(), " # variable \n");
            dumpInstructions(m_output_file.get(), riscvAsmPop(t0));
            // clang-format off
            constexpr const char *const riscv_assembly_local_constant =
                "    sw t0, %d(s0)\n"
                ;
            // clang-format on
            dumpInstructions(m_output_file.get(), riscv_assembly_local_constant,
                             m_stack_manager.offset(offset));
        }
    }
}

void CodeGenerator::visit(ConstantValueNode &p_constant_value) {
    auto type = p_constant_value.getType();
    auto constant = p_constant_value.getConstant();
    dumpInstructions(m_output_file.get(), " # constant \n");
    switch (type->value) {
    case Type::Value::Integer: {
        // clang-format off
        constexpr const char *const riscv_assembly_constant_int =
            "    li t0, %d\n"
            ;
        // clang-format on
        dumpInstructions(m_output_file.get(), riscv_assembly_constant_int,
                         constant->getIntValue());
        break;
    }
    default:
        throw "not implemented";
    }
    dumpInstructions(m_output_file.get(), riscvAsmPush(t0));
}

void CodeGenerator::visit(FunctionNode &p_function) {
    m_symbol_manager.pushScope(p_function.getSymbolTable());

    p_function.visitChildNodes(*this);

    m_symbol_manager.popScope();
}

void CodeGenerator::visit(CompoundStatementNode &p_compound_statement) {
    m_symbol_manager.pushScope(p_compound_statement.getSymbolTable());

    p_compound_statement.visitChildNodes(*this);

    m_symbol_manager.popScope();
}

void CodeGenerator::visit(PrintNode &p_print) {
    p_print.visitChildNodes(*this);
    // clang-format off
    constexpr const char *const riscv_assembly_call_print =
        riscvAsmPop(a0)
        "    call printInt\n"
        ;
    // clang-format on
    switch (p_print.getExpr()->getInferredType()->value) {
    case Type::Value::Integer:
        dumpInstructions(m_output_file.get(), riscv_assembly_call_print);
        break;
    default:
        throw "not implemented";
    }
}

void CodeGenerator::visit(BinaryOperatorNode &p_bin_op) {}

void CodeGenerator::visit(UnaryOperatorNode &p_un_op) {}

void CodeGenerator::visit(FunctionInvocationNode &p_func_invocation) {}

void CodeGenerator::visit(VariableReferenceNode &p_variable_ref) {
    dumpInstructions(m_output_file.get(), " # var ref \n");
    auto name = p_variable_ref.getNameString();
    auto entry = m_symbol_manager.lookup(name);

    // clang-format off
    constexpr const char *const riscv_assembly_var_ref_global =
        "    la t0, %s\n"
        ;
    constexpr const char *const riscv_assembly_var_ref_local =
        "    addi t0, s0, %d\n"
        ;
    // clang-format on
    if (entry->isGlobal()) {
        dumpInstructions(m_output_file.get(), riscv_assembly_var_ref_global,
                         name.c_str());
    } else {
        dumpInstructions(m_output_file.get(), riscv_assembly_var_ref_local,
                         m_stack_manager.offset(entry));
    }

    // clang-format off
    constexpr const char *const riscv_assembly_load_value =
        "    lw t1, 0(t0)\n"
        "    mv t0, t1\n"
        ;
    // clang-format on
    if (p_variable_ref.isValueUsage())
        dumpInstructions(m_output_file.get(), riscv_assembly_load_value);

    // TODO: array exprs

    dumpInstructions(m_output_file.get(), riscvAsmPush(t0));
}

void CodeGenerator::visit(AssignmentNode &p_assignment) {
    p_assignment.visitChildNodes(*this);

    dumpInstructions(m_output_file.get(), " # assign \n");
    // clang-format off
    constexpr const char *const riscv_assembly_assign =
        riscvAsmPop(t0)
        riscvAsmPop(t1)
        "    sw t0, 0(t1)\n"
        ;
    // clang-format on
    dumpInstructions(m_output_file.get(), riscv_assembly_assign);
}

void CodeGenerator::visit(ReadNode &p_read) {}

void CodeGenerator::visit(IfNode &p_if) {}

void CodeGenerator::visit(WhileNode &p_while) {}

void CodeGenerator::visit(ForNode &p_for) {
    m_symbol_manager.pushScope(p_for.getSymbolTable());

    p_for.visitChildNodes(*this);

    m_symbol_manager.popScope();
}

void CodeGenerator::visit(ReturnNode &p_return) {}
