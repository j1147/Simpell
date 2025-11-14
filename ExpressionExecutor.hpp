#pragma once
#include "ExpressionNode.hpp"
#include "Value.hpp"
#include "RoutineContext.hpp"
#include "ExpressionReader.hpp"

class ExpressionExecutor
{
public:
	const ExpressionNode* expression;

	ExpressionExecutor(const ExpressionNode* expression) : expression(expression) {};

	Value* execute(RoutineContext* routineContext, ExpressionReader::Context context) const;
	Value* mathOp(char op, const Value* lhs, const Value* rhs) const;
};