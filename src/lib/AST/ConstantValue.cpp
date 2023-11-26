#include "AST/ConstantValue.hpp"

ConstantValueNode::ConstantValueNode(const uint32_t line, const uint32_t col, int p_int)
    : ExpressionNode(line, col),
    type(Type::makeInteger()),
    value(p_int),
    value_str(std::to_string(p_int))
{}

ConstantValueNode::ConstantValueNode(const uint32_t line, const uint32_t col, float p_real)
    : ExpressionNode(line, col),
    type(Type::makeReal()),
    value(p_real),
    value_str(std::to_string(p_real))
{}

ConstantValueNode::ConstantValueNode(const uint32_t line, const uint32_t col, char *const p_str)
    : ExpressionNode(line, col),
    type(Type::makeString()),
    value(p_str),
    value_str(p_str)
{
    free(p_str);
}

ConstantValueNode::ConstantValueNode(const uint32_t line, const uint32_t col, bool p_bool)
    : ExpressionNode(line, col),
    type(Type::makeBoolean()),
    value(p_bool),
    value_str(p_bool ? "true" : "false")
{}
