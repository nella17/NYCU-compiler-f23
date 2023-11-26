#include "AST/operator.hpp"

OperatorNode::OperatorNode(const uint32_t line, const uint32_t col, Operator p_op):
    AstNode(line, col), ExpressionNode(line, col), op(p_op) {}

const char* to_cstring(Operator op) {
    switch (op) {
        case Operator::MUL:     return "*";
        case Operator::DIV:     return "/";
        case Operator::MOD:     return "mod";
        case Operator::ADD:     return "+";
        case Operator::SUB:     return "-";
        case Operator::OP_LT:   return "<";
        case Operator::OP_LTEQ: return "<=";
        case Operator::OP_NEQ:  return "<>";
        case Operator::OP_GTEQ: return ">=";
        case Operator::OP_GT:   return ">";
        case Operator::OP_EQ:   return "=";
        case Operator::AND:     return "and";
        case Operator::OR:      return "or";
        case Operator::NEG:     return "neg";
        case Operator::NOT:     return "not";
    }
    __builtin_unreachable();
}
