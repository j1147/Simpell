#pragma once
#include "Token.hpp"
#include <ctype.h>

bool Token::isNewline(char character)
{
	return character == newline || character == newline2;
}

bool Token::isWhitespace(char character)
{
	return character == space || character == tab || isNewline(character);
}

bool Token::isControlFlow(char character)
{
	switch (character)
	{
	case semicolon:
	case colon:
	case l_bracket:
	case r_bracket:
	case comma:
		return true;
	default:
		return false;
	}
}

bool Token::Operator::isOperator(char character)
{
	switch (character)
	{
	case assign:
	case add:
	case sub:
	case mul:
	case div:
	case invert:
	case mod:
	case op_and:
	case op_or:
	case dot:
	case l_parenthesis:
	case r_parenthesis:
		return true;
	default:
		return false;
	}
}

bool Token::isIdentifierValid(char character)
{
	return (isalpha(character) || character == Token::underscore)
		&& !isWhitespace(character)
		&& !isControlFlow(character)
		&& !Operator::isOperator(character);
}

bool Token::isNumber(char character)
{
	return isdigit(character);
}

string Token::getTokenName(token* token)
{
	switch (token->type)
	{
	case Token::Type::STRING:
		return "string";
	case Token::Type::NUMBER:
		return "number";
	case Token::Type::CONTROL_FLOW:
		return "control_flow";
	case Token::Type::OPERATOR:
		return "operator";
	default:
		return "unknown";
	}
}