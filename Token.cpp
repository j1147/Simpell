#pragma once
#include "Token.hpp"
#include <ctype.h>
#include <string>

using std::string;

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
	case q_mark:
		return true;
	default:
		return false;
	}
}

bool Token::Keyword::isKeyword(string str)
{
	return str == kw_true
		|| str == kw_false
		|| str == kw_routine
		|| str == kw_using
		|| str == kw_nothing
		|| str == kw_define
		|| str == kw_during
		|| str == kw_if
		|| str == kw_else
		|| str == kw_done
		|| str == kw_yield
		|| str == kw_call
		|| str == kw_with;
}

bool Token::Operator::isOperator(char character)
{
	switch (character)
	{
	case equal:
	case greater_than:
	case less_than:
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

bool Token::Operator::isUnaryOperator(char character)
{
	switch (character)
	{
	case invert:
		return true;
	default:
		return false;
	}
}

unsigned int Token::Operator::getPrecedence(char character)
{
	switch (character)
	{
	case equal:
	case less_than:
	case greater_than:
		return 0;
	case op_and:
	case op_or:
		return 1;
	case add:
	case sub:
		return 2;
	case mul:
	case div:
	case mod:
		return 3;
	case invert:
		return 4;
	case dot:
		return 5;
	default:
		return 0;
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
	case Token::Type::KEYWORD:
		return "keyword";
	default:
		return "unknown";
	}
}