#pragma once
#include "SyntaxParser.hpp"
#include "ExpressionNode.hpp"
#include "FunctionCallNode.hpp"

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
	FunctionCallNode* parseFunctionCall(ExpressionReader::Context context);
};