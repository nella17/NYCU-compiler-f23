#include "codegen/CodeGenerator.hpp"

#include "sema/error.hpp"
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

uint32_t curline = 1;
void logSource(FILE *p_out_file, uint32_t nxt) {
    while (curline <= nxt) {
        dumpInstructions(p_out_file, " # %u: %s \n", curline,
                         getSource(curline).c_str());
        curline++;
    }
}

// clang-format off
constexpr const char *const riscv_assembly_func_prologue =
    ".section    .text\n"
    "    .align 2\n"
    "    .global %1$s\n"
    "    .type %1$s, @function\n"
    "%1$s:\n"
    "    addi sp, sp, -128\n"
    "    sw ra, 124(sp)\n"
    "    sw s0, 120(sp)\n"
    "    addi s0, sp, 128\n"
    ;
constexpr const char *const riscv_assembly_func_epilogue =
    "    lw ra, 124(sp)\n"
    "    lw s0, 120(sp)\n"
    "    addi sp, sp, 128\n"
    "    jr ra\n"
    "    .size %1$s, .-%1$s\n"
    ;
// clang-format on

void CodeGenerator::visit(ProgramNode &p_program) {
    logSource(m_output_file.get(), p_program.getLocation().line);

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

    dumpInstructions(m_output_file.get(), riscv_assembly_func_prologue, "main");
    m_stack_manager.push(128);

    const_cast<CompoundStatementNode &>(p_program.getBody()).accept(*this);

    dumpInstructions(m_output_file.get(), riscv_assembly_func_epilogue, "main");
    m_stack_manager.pop();

    m_symbol_manager.popScope();
}

void CodeGenerator::visit(DeclNode &p_decl) {
    logSource(m_output_file.get(), p_decl.getLocation().line);

    p_decl.visitChildNodes(*this);
}

void CodeGenerator::visit(VariableNode &p_variable) {
    logSource(m_output_file.get(), p_variable.getLocation().line);

    auto name = p_variable.getNameString();
    auto type = p_variable.getType();
    auto constant = p_variable.getConstant();
    auto entry = m_symbol_manager.lookup(name);
    auto align = type->getAlign();
    auto size = type->getSize();
    if (entry->isGlobal()) {
        // clang-format off
        constexpr const char *const riscv_assembly_global_int =
            "    .global %1$s\n"
            "    .section %2$s\n"
            "    .align %3$d\n"
            "    .type %1$s, @object\n"
            "    .size %1$s, %4$d\n"
            "%1$s:\n"
            "    .word %5$d\n"
            ;
        // clang-format on
        const char *section = constant ? ".rodata" : ".bss";
        switch (type->value) {
        case Type::Value::Integer: {
            int value = constant ? constant->getIntValue() : 0;
            dumpInstructions(m_output_file.get(), riscv_assembly_global_int,
                             name.c_str(), section, align, size, value);
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
    logSource(m_output_file.get(), p_constant_value.getLocation().line);

    auto type = p_constant_value.getType();
    auto constant = p_constant_value.getConstant();
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
    logSource(m_output_file.get(), p_function.getLocation().line);

    m_symbol_manager.pushScope(p_function.getSymbolTable());

    p_function.visitChildNodes(*this);

    m_symbol_manager.popScope();
}

void CodeGenerator::visit(CompoundStatementNode &p_compound_statement) {
    logSource(m_output_file.get(), p_compound_statement.getLocation().line);

    m_symbol_manager.pushScope(p_compound_statement.getSymbolTable());

    p_compound_statement.visitChildNodes(*this);

    m_symbol_manager.popScope();
}

void CodeGenerator::visit(PrintNode &p_print) {
    logSource(m_output_file.get(), p_print.getLocation().line);

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

const char *genOpCode(Operator op) {
    switch (op) {
    case Operator::ADD:
        return "add";
    case Operator::SUB:
        return "sub";
    case Operator::MUL:
        return "mul";
    case Operator::DIV:
        return "div";
    case Operator::MOD:
        return "rem";
    case Operator::OP_LT:
    case Operator::OP_LTEQ:
    case Operator::OP_NEQ:
    case Operator::OP_GTEQ:
    case Operator::OP_GT:
    case Operator::OP_EQ:
        throw "not implemented";
    case Operator::AND:
    case Operator::OR:
        throw "not implemented";
    case Operator::NEG:
        throw "not implemented";
    case Operator::NOT:
        throw "not implemented";
    }
    __builtin_unreachable();
}

void CodeGenerator::visit(BinaryOperatorNode &p_bin_op) {
    logSource(m_output_file.get(), p_bin_op.getLocation().line);

    p_bin_op.visitChildNodes(*this);

    dumpInstructions(m_output_file.get(), riscvAsmPop(t0) riscvAsmPop(t1));
    auto op = p_bin_op.getOp();
    dumpInstructions(m_output_file.get(), "    %s t0, t1, t0\n", genOpCode(op));
    dumpInstructions(m_output_file.get(), riscvAsmPush(t0));
}

void CodeGenerator::visit(UnaryOperatorNode &p_un_op) {
    logSource(m_output_file.get(), p_un_op.getLocation().line);
}

void CodeGenerator::visit(FunctionInvocationNode &p_func_invocation) {
    logSource(m_output_file.get(), p_func_invocation.getLocation().line);
}

void CodeGenerator::visit(VariableReferenceNode &p_variable_ref) {
    logSource(m_output_file.get(), p_variable_ref.getLocation().line);

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
    logSource(m_output_file.get(), p_assignment.getLocation().line);

    p_assignment.visitChildNodes(*this);

    // clang-format off
    constexpr const char *const riscv_assembly_assign =
        riscvAsmPop(t0)
        riscvAsmPop(t1)
        "    sw t0, 0(t1)\n"
        ;
    // clang-format on
    dumpInstructions(m_output_file.get(), riscv_assembly_assign);
}

void CodeGenerator::visit(ReadNode &p_read) {
    logSource(m_output_file.get(), p_read.getLocation().line);
}

void CodeGenerator::visit(IfNode &p_if) {
    logSource(m_output_file.get(), p_if.getLocation().line);
}

void CodeGenerator::visit(WhileNode &p_while) {
    logSource(m_output_file.get(), p_while.getLocation().line);
}

void CodeGenerator::visit(ForNode &p_for) {
    logSource(m_output_file.get(), p_for.getLocation().line);

    m_symbol_manager.pushScope(p_for.getSymbolTable());

    p_for.visitChildNodes(*this);

    m_symbol_manager.popScope();
}

void CodeGenerator::visit(ReturnNode &p_return) {
    logSource(m_output_file.get(), p_return.getLocation().line);
}
