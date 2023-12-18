#include "sema/SemanticAnalyzer.hpp"
#include "visitor/AstNodeInclude.hpp"

void SemanticAnalyzer::dumpError() {
    for (auto error: errors)
        std::cerr << error;
}

void SemanticAnalyzer::visit(ProgramNode &p_program) {
    symbolmanager.pushGlobalScope();
    contexts.emplace_back(ContextKind::kProgram);
    retTypes.emplace_back(p_program.getType());

    symbolmanager.addSymbol(
        p_program.getNameString(),
        SymbolKind::kProgram,
        p_program.getType()
    );
    p_program.visitChildNodes(*this);

    retTypes.pop_back();
    contexts.pop_back();
    symbolmanager.popScope();
}

void SemanticAnalyzer::visit(DeclNode &p_decl) {
    p_decl.visitChildNodes(*this);
}

SymbolKind SemanticAnalyzer::varKind(VariableNode &p_variable) const {
    if (inFunction())
        return SymbolKind::kParameter;
    if (inFor())
        return SymbolKind::kLoopVar;
    if (p_variable.getConstant())
        return SymbolKind::kConstant;
    return SymbolKind::kVariable;
}

void SemanticAnalyzer::visit(VariableNode &p_variable) {
    auto entry = symbolmanager.addSymbol(
        p_variable.getNameString(),
        varKind(p_variable),
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
    p_constant_value.setInferType(p_constant_value.getType());
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
    retTypes.emplace_back(p_function.getType());

    p_function.visitChildNodes(*this);

    retTypes.pop_back();
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

    try {
        auto expr = p_print.getExpr();
        if (expr->isError()) throw nullptr;
        if (!expr->getInferredType()->isScalar()) {
            throw PrintTypeError(
                expr->getLocation()
            );
        }
    } catch (SemanticError* error) {
        if (error) errors.emplace_back(error);
    }
}

void SemanticAnalyzer::visit(BinaryOperatorNode &p_bin_op) {
    p_bin_op.visitChildNodes(*this);

    auto left = p_bin_op.getLeft(), right = p_bin_op.getRight();
    auto Ltype = left->getInferredType(), Rtype = right->getInferredType();
    auto op = p_bin_op.getOp();
    try {
        if (left->isError() or right->isError()) throw false;
        if (!Ltype or !Rtype) throw false;
        TypePtr type = nullptr;
        switch (op) {
            case Operator::ADD:
                if (Ltype->isString() and Rtype->isString()) {
                    type = Ltype;
                    break;
                }
            case Operator::SUB:
            case Operator::MUL:
            case Operator::DIV:
                if (Ltype->isInteger() and Rtype->isInteger()) {
                    type = Ltype;
                    break;
                }
                if (Ltype->capReal() and Rtype->capReal()) {
                    type = Ltype->isReal() ? Ltype : Rtype;
                    break;
                }
                throw true;
            case Operator::MOD:
                if (Ltype->isInteger() and Rtype->isInteger()) {
                    type = Ltype;
                    break;
                }
                throw true;
            case Operator::OP_LT:
            case Operator::OP_LTEQ:
            case Operator::OP_NEQ:
            case Operator::OP_GTEQ:
            case Operator::OP_GT:
            case Operator::OP_EQ:
                if (Ltype->isInteger() and Rtype->isInteger()) {
                    type = TypePtr(Type::makeBoolean());
                    break;
                }
                if (Ltype->capReal() and Rtype->capReal()) {
                    type = TypePtr(Type::makeBoolean());
                    break;
                }
                throw true;
            case Operator::AND:
            case Operator::OR:
                if (Ltype->isBool() and Rtype->isBool()) {
                    type = Ltype;
                    break;
                }
                throw true;
            default:
                throw true;
        }
        p_bin_op.setInferType(type);
    } catch (bool error) {
        p_bin_op.setError();
        if (error) {
            errors.emplace_back(
                InvalidBinaryOp(
                    p_bin_op.getLocation(),
                    op, Ltype, Rtype
                )
            );
        }
    }
}

void SemanticAnalyzer::visit(UnaryOperatorNode &p_un_op) {
    p_un_op.visitChildNodes(*this);

    auto expr = p_un_op.getExpr();
    auto type = expr->getInferredType();
    auto op = p_un_op.getOp();
    try {
        if (expr->isError() or !expr->getInferredType()) throw false;
        switch (op) {
            case Operator::NEG:
                if (type->isInteger() or type->isReal())
                    break;
                throw true;
            case Operator::NOT:
                if (type->isBool())
                    break;
                throw true;
            default:
                throw true;
        }
        p_un_op.setInferType(type);
    } catch (bool error) {
        p_un_op.setError();
        if (error) {
            errors.emplace_back(
                InvalidUnaryOp(
                    p_un_op.getLocation(),
                    op, type
                )
            );
        }
    }
}

void SemanticAnalyzer::visit(FunctionInvocationNode &p_func_invocation) {
    p_func_invocation.visitChildNodes(*this);

    try {
        auto entry = symbolmanager.lookup(p_func_invocation.getNameString());
        if (!entry) {
            throw UndeclaredError(
                p_func_invocation.getLocation(),
                p_func_invocation.getNameString()
            );
        }
        if (entry->isError()) throw nullptr;
        auto args = entry->getArgs();
        if (entry->getKind() != SymbolKind::kFunction or !args) {
            throw NonFunctionError(
                p_func_invocation.getLocation(),
                p_func_invocation.getNameString()
            );
        }
        auto& exprs = p_func_invocation.getExprs();
        auto& types = args->getTypes();
        if (exprs.size() != types.size()) {
            throw ArgsError(
                p_func_invocation.getLocation(),
                p_func_invocation.getNameString()
            );
        }
        for (size_t i = 0, sz = exprs.size(); i < sz; i++) {
            if (!(exprs[i]->getInferredType() <= types[i])) {
                throw IncompatibleParamError(
                    exprs[i]->getLocation(),
                    exprs[i]->getInferredType(),
                    types[i]
                );
            }
        }
        p_func_invocation.setInferType(entry->getType());
    } catch (SemanticError* error) {
        p_func_invocation.setError();
        if (error) errors.emplace_back(error);
    }
}

void SemanticAnalyzer::visit(VariableReferenceNode &p_variable_ref) {
    p_variable_ref.visitChildNodes(*this);

    auto entry = symbolmanager.lookup(p_variable_ref.getNameString());
    try {
        if (!entry) {
            throw UndeclaredError(
                p_variable_ref.getLocation(),
                p_variable_ref.getNameString()
            );
        }
        if (entry->isError()) throw nullptr;
        auto kind = entry->getKind();
        switch (kind) {
        case SymbolKind::kParameter:
        case SymbolKind::kVariable:
        case SymbolKind::kLoopVar:
        case SymbolKind::kConstant:
            break;
        default:
            throw NonVariableError(
                p_variable_ref.getLocation(),
                p_variable_ref.getNameString()
            );
        }
        auto type = std::make_shared<Type>(*entry->getType());
        for (auto expr: p_variable_ref.getExprs()) {
            if (expr->isError() or !expr->getInferredType()) throw nullptr;
            if (!expr->getInferredType()->isInteger())
                throw ArrayRefIntError(
                    expr->getLocation()
                );
            if (type->getDim().empty())
                throw OverArraySubError(
                    p_variable_ref.getLocation(),
                    p_variable_ref.getNameString()
                );
            type->popDim();
        }
        p_variable_ref.setInferType(type);
        p_variable_ref.setEntry(entry);
    } catch (SemanticError* error) {
        p_variable_ref.setError();
        if (error) errors.emplace_back(error);
    }
}

void SemanticAnalyzer::visit(AssignmentNode &p_assignment) {
    p_assignment.visitChildNodes(*this);

    try {
        auto var_ref = p_assignment.getVarRef();
        auto entry = var_ref->getEntry();
        if (var_ref->isError() or !entry) throw nullptr;
        if (!var_ref->getInferredType()->getDim().empty()) {
            throw ArrayAssignError(
                var_ref->getLocation()
            );
        }
        if (entry->getKind() == SymbolKind::kConstant) {
            throw ConstAssignError(
                var_ref->getLocation(),
                var_ref->getNameString()
            );
        }
        if (!inFor() and entry->getKind() == SymbolKind::kLoopVar) {
            throw LoopVarAssignError(
                var_ref->getLocation()
            );
        }
        auto expr = p_assignment.getExpr();
        if (expr->isError()) throw nullptr;
        if (!expr->getInferredType()->getDim().empty()) {
            throw ArrayAssignError(
                expr->getLocation()
            );
        }
        if (!(expr->getInferredType() <= var_ref->getInferredType())) {
            throw IncompatibleAssignError(
                p_assignment.getLocation(),
                var_ref->getInferredType(),
                expr->getInferredType()
            );
        }
    } catch (SemanticError* error) {
        if (error) errors.emplace_back(error);
    }
}

void SemanticAnalyzer::visit(ReadNode &p_read) {
    p_read.visitChildNodes(*this);

    try {
        auto var_ref = p_read.getVarRef();
        if (var_ref->isError()) throw nullptr;
        if (!var_ref->getInferredType()->isScalar()) {
            throw ReadTypeError(
                var_ref->getLocation()
            );
        }
        auto entry = symbolmanager.lookup(var_ref->getNameString());
        if (entry->isError()) throw nullptr;
        if (entry->getKind() == SymbolKind::kConstant or entry->getKind() == SymbolKind::kLoopVar) {
            throw ReadROError(
                var_ref->getLocation()
            );
        }
    } catch (SemanticError* error) {
        if (error) errors.emplace_back(error);
    }
}

void SemanticAnalyzer::visit(IfNode &p_if) {
    p_if.visitChildNodes(*this);

    try {
        auto expr = p_if.getExpr();
        auto type = expr->getInferredType();
        if (!type) throw nullptr;
        if (!type->isBool()) {
            throw ConditionTypeError(
                expr->getLocation()
            );
        }
    } catch (SemanticError* error) {
        if (error) errors.emplace_back(error);
    }
}

void SemanticAnalyzer::visit(WhileNode &p_while) {
    p_while.visitChildNodes(*this);

    try {
        auto expr = p_while.getExpr();
        auto type = expr->getInferredType();
        if (!type) throw nullptr;
        if (!type->isBool()) {
            throw ConditionTypeError(
                expr->getLocation()
            );
        }
    } catch (SemanticError* error) {
        if (error) errors.emplace_back(error);
    }
}

void SemanticAnalyzer::visit(ForNode &p_for) {
    symbolmanager.pushScope();
    contexts.emplace_back(ContextKind::kFor);

    p_for.visitChildNodes(*this);

    try {
        if (p_for.getBegin() > p_for.getEnd()) {
            throw LoopError(
                p_for.getLocation()
            );
        }
    } catch (SemanticError* error) {
        if (error) errors.emplace_back(error);
    }

    contexts.pop_back();
    symbolmanager.popScope();
}

void SemanticAnalyzer::visit(ReturnNode &p_return) {
    p_return.visitChildNodes(*this);

    try {
        if (retTypes.back()->isVoid()) {
            throw ReturnVoidError(
                p_return.getLocation()
            );
        }
        auto expr = p_return.getExpr();
        auto type = expr->getInferredType();
        if (!type) throw nullptr;
        if (!(type <= retTypes.back())) {
            throw ReturnTypeError(
                expr->getLocation(),
                type,
                retTypes.back()
            );
        }
    } catch (SemanticError* error) {
        if (error) errors.emplace_back(error);
    }
}
