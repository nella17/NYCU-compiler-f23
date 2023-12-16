#include "sema/SemanticAnalyzer.hpp"
#include "visitor/AstNodeInclude.hpp"

void SemanticAnalyzer::dumpError() {
    for (auto error: errors)
        std::cout << error;
}

void SemanticAnalyzer::visit(ProgramNode &p_program) {
    symbolmanager.pushGlobalScope();
    contexts.emplace_back(ContextKind::kProgram);

    symbolmanager.addSymbol(
        p_program.getNameString(),
        SymbolKind::kProgram,
        p_program.getType()
    );
    p_program.visitChildNodes(*this);

    contexts.pop_back();
    symbolmanager.popScope();
}

void SemanticAnalyzer::visit(DeclNode &p_decl) {
    p_decl.visitChildNodes(*this);
}

void SemanticAnalyzer::visit(VariableNode &p_variable) {
    auto kind = SymbolKind::kVariable;
    if (inFunction())
        kind = SymbolKind::kParameter;
    if (inFor())
        kind = SymbolKind::kLoopVar;
    if (p_variable.getConstant())
        kind = SymbolKind::kConstant;
    auto entry = symbolmanager.addSymbol(
        p_variable.getNameString(),
        kind,
        p_variable.getType(),
        p_variable.getConstant()
    );
    if (!entry) {
        errors.emplace_back(
            SymbolRedeclError(
                p_variable.getLocation(),
                p_variable.getNameString()
            )
        );
    }

    p_variable.visitChildNodes(*this);

    if (!p_variable.getType()->checkDim()) {
        entry->setError();
        errors.emplace_back(
            ArrayDeclGT0Error(
                p_variable.getLocation(),
                p_variable.getNameString()
            )
        );
    }
}

void SemanticAnalyzer::visit(ConstantValueNode &p_constant_value) {
    // TODO
}

void SemanticAnalyzer::visit(FunctionNode &p_function) {
    auto entry = symbolmanager.addSymbol(
        p_function.getNameString(),
        SymbolKind::kFunction,
        p_function.getType(),
        p_function.getArgs()
    );
    if (!entry) {
        errors.emplace_back(
            SymbolRedeclError(
                p_function.getLocation(),
                p_function.getNameString()
            )
        );
    }

    symbolmanager.pushScope();
    contexts.emplace_back(ContextKind::kFunction);

    p_function.visitChildNodes(*this);

    contexts.pop_back();
    symbolmanager.popScope();
}

void SemanticAnalyzer::visit(CompoundStatementNode &p_compound_statement) {
    if (!inFunction()) symbolmanager.pushScope();
    contexts.emplace_back(ContextKind::kCompound);

    p_compound_statement.visitChildNodes(*this);

    contexts.pop_back();
    if (!inFunction()) symbolmanager.popScope();
}

void SemanticAnalyzer::visit(PrintNode &p_print) {
    p_print.visitChildNodes(*this);
    // TODO
}

void SemanticAnalyzer::visit(BinaryOperatorNode &p_bin_op) {
    p_bin_op.visitChildNodes(*this);
    // TODO
}

void SemanticAnalyzer::visit(UnaryOperatorNode &p_un_op) {
    p_un_op.visitChildNodes(*this);
    // TODO
}

void SemanticAnalyzer::visit(FunctionInvocationNode &p_func_invocation) {
    p_func_invocation.visitChildNodes(*this);
    // TODO
}

void SemanticAnalyzer::visit(VariableReferenceNode &p_variable_ref) {
    p_variable_ref.visitChildNodes(*this);

    auto entry = symbolmanager.lookup(p_variable_ref.getNameString());
    if (!entry) {
        errors.emplace_back(
            UndeclaredError(
                p_variable_ref.getLocation(),
                p_variable_ref.getNameString()
            )
        );
    } else if (!entry->isError()) {
        auto kind = entry->getKind();
        switch (kind) {
        case SymbolKind::kParameter:
        case SymbolKind::kVariable:
        case SymbolKind::kLoopVar:
        case SymbolKind::kConstant: {
            bool error = false;
            for (auto expr: p_variable_ref.getExprs()) {
                if (!expr->getType()->isInteger()) {
                    errors.emplace_back(
                        ArrayRefIntError(
                            expr->getLocation()
                        )
                    );
                    error = true;
                    break;
                }
            }
            if (error) break;
            if (p_variable_ref.getExprs().size() > entry->getType()->getSize()) {
                errors.emplace_back(
                    OverArraySubError(
                        p_variable_ref.getLocation(),
                        p_variable_ref.getNameString()
                    )
                );
            }
            break;
        }
        default:
            errors.emplace_back(
                NonVariableError(
                    p_variable_ref.getLocation(),
                    p_variable_ref.getNameString()
                )
            );
        }
    }
}

void SemanticAnalyzer::visit(AssignmentNode &p_assignment) {
    p_assignment.visitChildNodes(*this);
    // TODO
}

void SemanticAnalyzer::visit(ReadNode &p_read) {
    p_read.visitChildNodes(*this);
    // TODO
}

void SemanticAnalyzer::visit(IfNode &p_if) {
    p_if.visitChildNodes(*this);
    // TODO
}

void SemanticAnalyzer::visit(WhileNode &p_while) {
    p_while.visitChildNodes(*this);
    // TODO
}

void SemanticAnalyzer::visit(ForNode &p_for) {
    symbolmanager.pushScope();
    contexts.emplace_back(ContextKind::kFor);

    p_for.visitChildNodes(*this);

    contexts.pop_back();
    symbolmanager.popScope();
}

void SemanticAnalyzer::visit(ReturnNode &p_return) {
    p_return.visitChildNodes(*this);
    // TODO
}
