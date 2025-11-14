#pragma once
#include "SyntaxParser.hpp"
#include "ExpressionNode.hpp"
#include "RoutineCallNode.hpp"

class ExpressionReader
{
public:
	enum class Context {
		INLINE,
		FUNCTION_ARGUMENT,
		CONDITION
	};
	SyntaxParser* parser;

	ExpressionReader(SyntaxParser* parser) : parser(parser) {};
	ExpressionNode* parseExpression(ExpressionReader::Context context);
	RoutineCallNode* parseFunctionCall(ExpressionReader::Context context);
};