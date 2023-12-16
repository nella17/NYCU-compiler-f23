#include "AST/ConstantValue.hpp"

ConstantValueNode::ConstantValueNode(const uint32_t line, const uint32_t col, int p_int)
    : ExpressionNode(line, col),
    value(p_int),
    value_str(std::to_string(p_int))
{
    type.reset(Type::makeInteger());
}

ConstantValueNode::ConstantValueNode(const uint32_t line, const uint32_t col, float p_real)
    : ExpressionNode(line, col),
    value(p_real),
    value_str(std::to_string(p_real))
{
    type.reset(Type::makeReal());
}

ConstantValueNode::ConstantValueNode(const uint32_t line, const uint32_t col, char *const p_str)
    : ExpressionNode(line, col),
    value(p_str),
    value_str(p_str)
{
    type.reset(Type::makeString());
    free(p_str);
}

ConstantValueNode::ConstantValueNode(const uint32_t line, const uint32_t col, bool p_bool)
    : ExpressionNode(line, col),
    value(p_bool),
    value_str(p_bool ? "true" : "false")
{
    type.reset(Type::makeBoolean());
}
