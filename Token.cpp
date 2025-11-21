#pragma once
#include "_global.hpp"
#include "Token.hpp"
#include <ctype.h>
#include <string>

using std::string;

bool Token::equals(Token::token* a, Token::token* b)
{
	if (a == b)
		return true;
	if ((!a && b) || (!b && a))
		return false;
	if (a && b)
		return a->content == b->content && a->type == b->type;
	else
		return false;
}
bool Token::equals(Token::token* a, const Token::token* b)
{
	if (a == b)
		return true;
	if ((!a && b) || (!b && a))
		return false;
	if (a && b)
		return a->content == b->content && a->type == b->type;
	else
		return false;
}

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
	case comma:
		return true;
	default:
		return false;
	}
}

bool Token::Keyword::isKeyword(string str)
{
	return str == kw_true.content
		|| str == kw_false.content
		|| str == kw_routine.content
		|| str == kw_using.content
		|| str == kw_nothing.content
		|| str == kw_define.content
		|| str == kw_during.content
		|| str == kw_if.content
		|| str == kw_else.content
		|| str == kw_done.content
		|| str == kw_yield.content
		|| str == kw_call.content
		|| str == kw_with.content;
}

bool Token::Operator::isOperator(char character)
{
	return character == equal.first()
		|| character == less_than.first()
		|| character == greater_than.first()
		|| character == add.first()
		|| character == sub.first()
		|| character == mul.first()
		|| character == div.first()
		|| character == mod.first()
		|| character == invert.first()
		|| character == op_and.first()
		|| character == op_or.first()
		//|| character == dot.first()
		|| character == l_parenthesis.first()
		|| character == r_parenthesis.first();
}

bool Token::Operator::isUnaryOperator(char character)
{
	return character == invert.first();
}

unsigned int Token::Operator::getPrecedence(char character)
{
	if (character == equal.first()
		|| character == op_or.first()
		|| character == op_and.first())
		return 0;

	if (character == less_than.first()
		|| character == greater_than.first())
		return 1;

	if (character == add.first()
		|| character == sub.first())
		return 2;

	if (character == mul.first()
		|| character == div.first()
		|| character == mod.first())
		return 3;

	if (character == invert.first())
		return 4;

	//if (character == dot.first())
	//	return 5;

	return 0;
}

const Token::token* Token::Operator::getOperator(char character)
{
	for (const Token::token& token : OPERATOR_LIST)
		if (token.first() == character)
			return &token;
	return nullptr;
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
	case Token::Type::NAME:
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