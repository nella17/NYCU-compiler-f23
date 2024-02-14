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
                             const std::string &save_path,
                             SymbolManager &p_symbol_manager)
    : m_symbol_manager(p_symbol_manager), m_source_file_path(source_file_path) {
    m_symbol_manager.dumpOff();
    // FIXME: assume that the source file is always xxxx.p
    fs::path real_path = save_path.empty() ? std::string{"."} : save_path;
    fs::path source_path = source_file_path;
    source_path.replace_extension(".S");
    fs::path output_file_path = real_path / source_path.filename();
    auto file = fopen(output_file_path.c_str(), "w");
    assert(file && "Failed to open output file");
    m_output_file.reset(file);
}

uint32_t curline = 1;
void logSource(FILE *p_out_file, uint32_t nxt) {
    while (curline <= nxt) {
        dumpInstructions(p_out_file, " # %u: %s \n", curline,
                         getSource(curline).c_str());
        curline++;
    }
}

std::string pad = "    ";
std::string convertIntToReal(int reg) {
    return pad + "fcvt.s.w " + "ft" + std::to_string(reg) + ", t" +
           std::to_string(reg) + "\n";
}
std::string genOpCode(const char *int_op,
                      const char *real_op,
                      TypePtr Ltype,
                      TypePtr Rtype) {
    if (Ltype->isInteger() and Rtype->isInteger())
        return pad + int_op + " t0, t0, t1\n";
    if (Ltype->capReal() and Rtype->capReal()) {
        std::string code = "";
        if (Ltype->isInteger())
            code += convertIntToReal(0);
        if (Rtype->isInteger())
            code += convertIntToReal(1);
        code += pad + real_op + " ft0, ft0, ft1\n";
        return code;
    }
    throw std::runtime_error("unreachable (genOpCode)");
}

std::string SUB(TypePtr Ltype, TypePtr Rtype) {
    return genOpCode("sub", "fsub.s", Ltype, Rtype);
}

std::string SLT(TypePtr Ltype, TypePtr Rtype) {
    if (Ltype->isInteger() and Rtype->isInteger())
        return pad + "slt t0, t0, t1\n";
    if (Ltype->capReal() and Rtype->capReal()) {
        std::string code = "";
        if (Ltype->isInteger())
            code += convertIntToReal(0);
        if (Rtype->isInteger())
            code += convertIntToReal(1);
        code += pad + "flt.s ft0, ft0, ft1\n";
        return code;
    }
    throw std::runtime_error("unreachable (SLT)");
}

std::string SGT(TypePtr Ltype, TypePtr Rtype) {
    if (Ltype->isInteger() and Rtype->isInteger())
        return pad + "sgt t0, t0, t1\n";
    if (Ltype->capReal() and Rtype->capReal()) {
        std::string code = "";
        if (Ltype->isInteger())
            code += convertIntToReal(0);
        if (Rtype->isInteger())
            code += convertIntToReal(1);
        code += pad + "fgt.s t0, ft0, ft1\n";
        return code;
    }
    throw std::runtime_error("unreachable (SGT)");
}

std::string SEQ(TypePtr Ltype, TypePtr Rtype) {
    if (Ltype->isInteger() and Rtype->isInteger())
        return SUB(Ltype, Rtype) + pad + "seqz t0, t0\n";
    if (Ltype->capReal() and Rtype->capReal()) {
        std::string code = "";
        if (Ltype->isInteger())
            code += convertIntToReal(0);
        if (Rtype->isInteger())
            code += convertIntToReal(1);
        code += pad + "feq.s t0, ft0, ft1\n";
        return code;
    }
    throw std::runtime_error("unreachable (SEQ)");
}

const char XORI[] = "    xori t0, t0, 1\n";

std::string genOpCode(Operator op, TypePtr Ltype, TypePtr Rtype = nullptr) {
    switch (op) {
    case Operator::ADD:
        return genOpCode("add", "fadd.s", Ltype, Rtype);
    case Operator::SUB:
        return SUB(Ltype, Rtype);
    case Operator::MUL:
        return genOpCode("mul", "fmul.s", Ltype, Rtype);
    case Operator::DIV:
        return genOpCode("div", "fdiv.s", Ltype, Rtype);
    case Operator::MOD:
        return "    rem t0, t0, t1\n";
    case Operator::OP_LT:
        return SLT(Ltype, Rtype);
    case Operator::OP_LTEQ:
        return SGT(Ltype, Rtype) + XORI;
    case Operator::OP_NEQ:
        return SEQ(Ltype, Rtype) + XORI;
    case Operator::OP_GTEQ:
        return SLT(Ltype, Rtype) + XORI;
    case Operator::OP_GT:
        return SGT(Ltype, Rtype);
    case Operator::OP_EQ:
        return SEQ(Ltype, Rtype);
    case Operator::AND:
        return "    and t0, t0, t1\n";
    case Operator::OR:
        return "    or t0, t0, t1\n";
    case Operator::NEG:
        if (Ltype->isInteger())
            return "    neg t0, t0\n";
        if (Ltype->isReal())
            return "    fneg.s ft0, ft0\n";
        throw std::runtime_error("unreachable (genOpCode::NEG)");
    case Operator::NOT:
        return "    xori t0, t0, 1\n";
    }
    __builtin_unreachable();
}

void CodeGenerator::dumpSymbol(std::string name) {
    auto entry = m_symbol_manager.lookup(name);
    return dumpSymbol(entry);
}

void CodeGenerator::dumpSymbol(SymbolEntryPtr entry) {
    dumpInstructions(m_output_file.get(), " # %s\n", entry->getName().c_str());
    // clang-format off
    constexpr const char *const riscv_assembly_var_ref_global =
        "    la t0, %s\n";
    constexpr const char *const riscv_assembly_var_ref_local =
        "    addi t0, s0, %d\n";
    // clang-format on
    if (entry->isGlobal()) {
        dumpInstructions(m_output_file.get(), riscv_assembly_var_ref_global,
                         entry->getName().c_str());
    } else {
        dumpInstructions(m_output_file.get(), riscv_assembly_var_ref_local,
                         m_stack_manager.offset(entry));
    }
}

void CodeGenerator::loadInt(const char *target) {
    // clang-format off
    constexpr const char *const riscv_assembly_load_int =
        "    lw %s, 0(t0)\n";
    // clang-format on
    dumpInstructions(m_output_file.get(), riscv_assembly_load_int, target);
}

void CodeGenerator::loadReal(const char *target) {
    // clang-format off
    constexpr const char *const riscv_assembly_load_real =
        "    flw %s, 0(t0)\n";
    // clang-format on
    dumpInstructions(m_output_file.get(), riscv_assembly_load_real, target);
}

void CodeGenerator::branchLabel(std::string label, bool pop) {
    // clang-format off
    constexpr const char *const riscv_assembly_branch =
        "    beqz t0, %s\n";
    // clang-format on
    if (pop)
        popt(0);
    dumpInstructions(m_output_file.get(), riscv_assembly_branch, label.c_str());
}

void CodeGenerator::dumpLabel(std::string label) {
    // clang-format off
    constexpr const char *const riscv_assembly_label =
        "%s:\n";
    // clang-format on
    dumpInstructions(m_output_file.get(), riscv_assembly_label, label.c_str());
}
void CodeGenerator::jumpLabel(std::string label) {
    // clang-format off
    constexpr const char *const riscv_assembly_jump =
        "    j %s\n";
    // clang-format on
    dumpInstructions(m_output_file.get(), riscv_assembly_jump, label.c_str());
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

    // clang-format off
    constexpr const char *const riscv_assembly_real_literal =
        "    .section .rodata\n"
        "    .align 2\n"
        "%s:\n"
        "    .float %f\n";
    constexpr const char *const riscv_assembly_string_literal =
        "    .section .rodata\n"
        "    .align 2\n"
        "%s:\n"
        "    .string \"%s\"\n";
    // clang-format on
    for (auto constant : m_symbol_manager.getConstants()) {
        switch (constant->getType()->value) {
        case Type::Value::Real: {
            auto label = genLabel();
            constant->setLabel(label);
            dumpInstructions(m_output_file.get(), riscv_assembly_real_literal,
                             label.c_str(), constant->getRealValue());
            break;
        }
        case Type::Value::String: {
            auto label = genLabel();
            constant->setLabel(label);
            dumpInstructions(m_output_file.get(), riscv_assembly_string_literal,
                             label.c_str(), constant->getStringValue().c_str());
            break;
        }
        default:;
        }
    }

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
        const char *section = constant ? ".rodata" : ".bss";
        // clang-format off
        constexpr const char *const riscv_assembly_global =
            "    .global %1$s\n"
            "    .section %2$s\n"
            "    .align %3$d\n"
            "    .type %1$s, @object\n"
            "    .size %1$s, %4$d\n"
            "%1$s:\n"
            "    .word %5$s\n"
            ;
        // clang-format on
        std::string value = "0";
        if (constant) {
            switch (type->value) {
            case Type::Value::Integer:
                value = std::to_string(constant->getIntValue());
                break;
            case Type::Value::Real: {
                union {
                    int integer;
                    float real;
                } v{.real = constant->getRealValue()};
                value = std::to_string(v.integer);
                break;
            }
            case Type::Value::Boolean:
                value = std::to_string(constant->getBooleanValue());
                break;
            case Type::Value::String:
                value = constant->getLabel();
                break;
            default:
                throw std::runtime_error("not implemented (global variable)");
            }
        }
        dumpInstructions(m_output_file.get(), riscv_assembly_global,
                         name.c_str(), section, align, size, value.c_str());
    } else {
        auto offset = m_stack_manager.add(size);
        entry->setOffset(offset);
        switch (entry->getKind()) {
        case SymbolKind::kConstant: {
            p_variable.visitChildNodes(*this);
            // clang-format off
            constexpr const char *const riscv_assembly_local_constant =
                riscvAsmPop(t0)
                "    sw t0, %d(s0)\n";
            // clang-format on
            dumpInstructions(m_output_file.get(), riscv_assembly_local_constant,
                             m_stack_manager.offset(offset));
            break;
        }
        case SymbolKind::kParameter: {
            // clang-format off
            constexpr const char *const riscv_assembly_arg_reg =
                "    sw a%d, %d(s0)\n";
            constexpr const char *const riscv_assembly_arg_reg_f =
                "    fsw fa%d, %d(s0)\n";
            constexpr const char *const riscv_assembly_arg_stack =
                "    lw t0, %d(s0)\n"
                "    sw t0, %d(s0)\n";
            // clang-format on
            if (m_parameter_cnt < 8) {
                if (type->isReal()) {
                    dumpInstructions(m_output_file.get(),
                                     riscv_assembly_arg_reg_f, m_parameter_cnt,
                                     m_stack_manager.offset(offset));
                } else {
                    dumpInstructions(m_output_file.get(),
                                     riscv_assembly_arg_reg, m_parameter_cnt,
                                     m_stack_manager.offset(offset));
                }
            } else {
                dumpInstructions(m_output_file.get(), riscv_assembly_arg_stack,
                                 4 * (m_parameter_cnt - 8),
                                 m_stack_manager.offset(offset));
            }
            m_parameter_cnt++;
            break;
        }
        default:;
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
            riscvAsmPush(t0);
        // clang-format on
        dumpInstructions(m_output_file.get(), riscv_assembly_constant_int,
                         constant->getIntValue());
        break;
    }
    case Type::Value::Real: {
        // clang-format off
        constexpr const char *const riscv_assembly_constant_real =
            "    lui t0, %%hi(%1$s)\n"
            "    flw ft0, %%lo(%1$s)(t0)\n"
            riscvAsmPushF(ft0);
        // clang-format on
        dumpInstructions(m_output_file.get(), riscv_assembly_constant_real,
                         constant->getLabel().c_str());
        break;
    }
    case Type::Value::String: {
        // clang-format off
        constexpr const char *const riscv_assembly_constant_string =
            "    lui t1, %%hi(%1$s)\n"
            "    addi t0, t1, %%lo(%1$s)\n"
            riscvAsmPush(t0);
        // clang-format on
        dumpInstructions(m_output_file.get(), riscv_assembly_constant_string,
                         constant->getLabel().c_str());
        break;
    }
    case Type::Value::Boolean: {
        // clang-format off
        constexpr const char *const riscv_assembly_constant_boolean =
            "    li t0, %d\n"
            riscvAsmPush(t0);
        // clang-format on
        dumpInstructions(m_output_file.get(), riscv_assembly_constant_boolean,
                         constant->getBooleanValue());
        break;
    }
    default:
        throw std::runtime_error("not implemented (constant)");
    }
}

void CodeGenerator::visit(FunctionNode &p_function) {
    logSource(m_output_file.get(), p_function.getLocation().line);

    if (!p_function.getBody())
        return;

    m_symbol_manager.pushScope(p_function.getSymbolTable());

    dumpInstructions(m_output_file.get(), riscv_assembly_func_prologue,
                     p_function.getNameCString());
    m_stack_manager.push(128);

    m_parameter_cnt = 0;
    p_function.visitChildNodes(*this);
    dumpInstructions(m_output_file.get(), riscv_assembly_func_epilogue,
                     p_function.getNameCString());
    m_stack_manager.pop();

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
        "    call %s\n";
    constexpr const char *const riscv_assembly_call_print_f =
        riscvAsmPopF(fa0)
        "    call %s\n";
    // clang-format on
    dumpInstructions(m_output_file.get(), " # call print\n");
    switch (p_print.getExpr()->getInferredType()->value) {
    case Type::Value::Integer:
        dumpInstructions(m_output_file.get(), riscv_assembly_call_print,
                         "printInt");
        break;
    case Type::Value::Real:
        dumpInstructions(m_output_file.get(), riscv_assembly_call_print_f,
                         "printReal");
        break;
    case Type::Value::String:
        dumpInstructions(m_output_file.get(), riscv_assembly_call_print,
                         "printString");
        break;
    default:
        throw std::runtime_error("not implemented (print)");
    }
}

void CodeGenerator::visit(BinaryOperatorNode &p_bin_op) {
    logSource(m_output_file.get(), p_bin_op.getLocation().line);

    p_bin_op.visitChildNodes(*this);

    auto left = p_bin_op.getLeft(), right = p_bin_op.getRight();
    auto Ltype = left->getInferredType(), Rtype = right->getInferredType();
    auto op = p_bin_op.getOp();

    dumpInstructions(m_output_file.get(), " # %s\n", to_cstring(op));
    Rtype->isReal() ? popft(1) : popt(1);
    Ltype->isReal() ? popft(0) : popt(0);
    dumpInstructions(m_output_file.get(), "%s",
                     genOpCode(op, Ltype, Rtype).c_str());
    p_bin_op.getInferredType()->isReal() ? pushft(0) : pusht(0);
}

void CodeGenerator::visit(UnaryOperatorNode &p_un_op) {
    logSource(m_output_file.get(), p_un_op.getLocation().line);

    p_un_op.visitChildNodes(*this);
    auto expr = p_un_op.getExpr();
    auto type = expr->getInferredType();
    auto op = p_un_op.getOp();

    dumpInstructions(m_output_file.get(), " # %s\n", to_cstring(op));
    type->isReal() ? popft(0) : popt(0);
    dumpInstructions(m_output_file.get(), "%s", genOpCode(op, type).c_str());
    p_un_op.getInferredType()->isReal() ? pushft(0) : pusht(0);
}

void CodeGenerator::visit(FunctionInvocationNode &p_func_invocation) {
    logSource(m_output_file.get(), p_func_invocation.getLocation().line);

    // clang-format off
    constexpr const char *const riscv_assembly_swap =
        "    lw t0, %1$d(sp)\n"
        "    lw t1, %2$d(sp)\n"
        "    sw t0, %2$d(sp)\n"
        "    sw t1, %1$d(sp)\n";
    constexpr const char *const riscv_assembly_pop_arg = riscvAsmPop(a%d);
    constexpr const char *const riscv_assembly_pop_arg_f = riscvAsmPopF(fa%d);
    constexpr const char *const riscv_assembly_call_func =
        "    call %s\n";
    constexpr const char *const riscv_assembly_mov_sp =
        "    addi sp, sp, %d\n";
    constexpr const char *const riscv_assembly_push_a0 =
        riscvAsmPush(a0);
    constexpr const char *const riscv_assembly_push_fa0 =
        riscvAsmPushF(fa0);
    // clang-format on
    auto name = p_func_invocation.getNameString();
    auto entry = m_symbol_manager.lookup(name);
    auto types = entry->getArgs()->getTypes();
    auto ret_type = p_func_invocation.getInferredType();
    int size = types.size();

    p_func_invocation.visitChildNodes(*this);
    dumpInstructions(m_output_file.get(), " # call %s\n", name.c_str());
    for (int l = 0, r = size - 1; l < r; l++, r--)
        dumpInstructions(m_output_file.get(), riscv_assembly_swap, 4 * l,
                         4 * r);
    for (int i = 0; i < size; i++)
        if (i < 8) {
            if (types[i]->isReal()) {
                // TODO: convert int to real
                dumpInstructions(m_output_file.get(), riscv_assembly_pop_arg_f,
                                 i);
            } else {
                dumpInstructions(m_output_file.get(), riscv_assembly_pop_arg,
                                 i);
            }
        }
    dumpInstructions(m_output_file.get(), riscv_assembly_call_func,
                     p_func_invocation.getNameCString());
    if (size > 8)
        dumpInstructions(m_output_file.get(), riscv_assembly_mov_sp,
                         4 * (size - 8));
    if (ret_type->isReal())
        dumpInstructions(m_output_file.get(), riscv_assembly_push_fa0);
    else if (not ret_type->isVoid())
        dumpInstructions(m_output_file.get(), riscv_assembly_push_a0);
}

void CodeGenerator::visit(VariableReferenceNode &p_variable_ref) {
    logSource(m_output_file.get(), p_variable_ref.getLocation().line);

    auto name = p_variable_ref.getNameString();
    auto entry = m_symbol_manager.lookup(name);
    dumpSymbol(entry);
    if (entry->isPointer())
        loadInt("t0");
    pusht(0);

    // clang-format off
    constexpr const char *const riscv_assembly_array =
        riscvAsmPop(t1)
        riscvAsmPop(t0)
        "    li t2, %d\n"
        "    mul t1, t1, t2\n"
        "    add t0, t0, t1\n"
        riscvAsmPush(t0);
    // clang-format on
    auto type = entry->getType()->copy();
    for (auto expr : p_variable_ref.getExprs()) {
        expr->accept(*this);
        type->popDim();
        dumpInstructions(m_output_file.get(), " # %s\n",
                         type->getNameString().c_str());
        dumpInstructions(m_output_file.get(), riscv_assembly_array,
                         type->getSize());
    }

    if (p_variable_ref.isValueUsage() and type->isNotArray()) {
        dumpInstructions(m_output_file.get(), " # load value\n");
        popt(0);
        if (type->isReal()) {
            loadReal("ft0");
            pushft(0);
        } else {
            loadInt("t0");
            pusht(0);
        }
    }
}

void CodeGenerator::visit(AssignmentNode &p_assignment) {
    logSource(m_output_file.get(), p_assignment.getLocation().line);

    p_assignment.visitChildNodes(*this);

    // clang-format off
    constexpr const char *const riscv_assembly_assign =
        riscvAsmPop(t0)
        riscvAsmPop(t1)
        "    sw t0, 0(t1)\n";
    constexpr const char *const riscv_assembly_assign_f =
        riscvAsmPopF(ft0)
        riscvAsmPop(t1)
        "    fsw ft0, 0(t1)\n";
    // clang-format on
    dumpInstructions(m_output_file.get(), " # assign\n");
    if (p_assignment.getVarRef()->getInferredType()->isReal())
        dumpInstructions(m_output_file.get(), riscv_assembly_assign_f);
    else
        dumpInstructions(m_output_file.get(), riscv_assembly_assign);
}

void CodeGenerator::visit(ReadNode &p_read) {
    logSource(m_output_file.get(), p_read.getLocation().line);

    p_read.visitChildNodes(*this);
    // clang-format off
    constexpr const char *const riscv_assembly_call_print =
        "    call readInt\n"
        riscvAsmPop(t0)
        "    sw a0, 0(t0)\n"
        ;
    // clang-format on
    switch (p_read.getVarRef()->getInferredType()->value) {
    case Type::Value::Integer:
        dumpInstructions(m_output_file.get(), riscv_assembly_call_print);
        break;
    default:
        throw std::runtime_error("not implemented (read)");
    }
}

void CodeGenerator::visit(IfNode &p_if) {
    logSource(m_output_file.get(), p_if.getLocation().line);

    auto end_label = genLabel();
    p_if.getExpr()->accept(*this);
    branchLabel(end_label);
    p_if.getTrueBody()->accept(*this);
    if (p_if.getFalseBody()) {
        auto false_label = end_label;
        end_label = genLabel();
        jumpLabel(end_label);
        dumpLabel(false_label);
        p_if.getFalseBody()->accept(*this);
    }
    dumpLabel(end_label);
}

void CodeGenerator::visit(WhileNode &p_while) {
    logSource(m_output_file.get(), p_while.getLocation().line);

    auto begin_label = genLabel(), end_label = genLabel();

    dumpLabel(begin_label);
    p_while.getExpr()->accept(*this);
    branchLabel(end_label);
    p_while.visitBodyChildNodes(*this);
    jumpLabel(begin_label);
    dumpLabel(end_label);
}

void CodeGenerator::visit(ForNode &p_for) {
    logSource(m_output_file.get(), p_for.getLocation().line);

    m_symbol_manager.pushScope(p_for.getSymbolTable());

    p_for.visitHeadChildNodes(*this);
    auto begin_label = genLabel(), end_label = genLabel();
    auto name = p_for.getLoopVarRef()->getNameString();
    // clang-format off
    constexpr const char *const riscv_assembly_cond =
        "    li t1, %d\n"
        "    slt t0, t0, t1\n";
    constexpr const char *const riscv_assembly_for_inc =
        "    addi t1, t1, 1\n"
        "    sw t1, 0(t0)\n";
    // clang-format on

    dumpLabel(begin_label);
    dumpSymbol(name);
    loadInt("t0");
    dumpInstructions(m_output_file.get(), riscv_assembly_cond, p_for.getEnd());
    branchLabel(end_label, false);
    p_for.visitBodyChildNodes(*this);
    dumpSymbol(name);
    loadInt("t1");
    dumpInstructions(m_output_file.get(), riscv_assembly_for_inc);
    jumpLabel(begin_label);
    dumpLabel(end_label);

    m_symbol_manager.popScope();
}

void CodeGenerator::visit(ReturnNode &p_return) {
    logSource(m_output_file.get(), p_return.getLocation().line);

    p_return.visitChildNodes(*this);
    // clang-format off
    constexpr const char *const riscv_assembly_return =
        riscvAsmPop(a0);
    constexpr const char *const riscv_assembly_return_f =
        riscvAsmPopF(fa0);
    // clang-format on
    if (p_return.getExpr()->getInferredType()->isReal())
        dumpInstructions(m_output_file.get(), riscv_assembly_return_f);
    else
        dumpInstructions(m_output_file.get(), riscv_assembly_return);
}
