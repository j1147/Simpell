#pragma once
#include "Tokenizer.hpp"
#include "CodeWrapper.hpp"
#include "Token.hpp"
#include <vector>
#include <string>
#include <stdexcept>
#include <format>

#include <iostream>

using std::vector;
using std::string;

vector<Token::token*>* Tokenizer::scan()
{
	CodeWrapper* wrapper = this->wrapper;
	int i = wrapper->pos;
	const long length = wrapper->length;
	const char* code = wrapper->code;

	vector<Token::token*>* tokens = new vector<Token::token*>();

	while (wrapper->pos < length)
	{
		wrapper->skipWhitespaces();
		if (wrapper->pos >= length)
			break;

		i = wrapper->pos;
		if (Token::isIdentifierValid(code[i]))
		{
			tokens->push_back(readIdentifier());
			continue;
		}

		if (Token::isNumber(code[i]))
		{
			tokens->push_back(readNumber());
			continue;
		}

		if (Token::isControlFlow(code[i]))
		{
			tokens->push_back(new Token::token(Token::Type::CONTROL_FLOW, code[i]));
			++wrapper->pos;
			continue;
		}

		if (Token::Operator::isOperator(code[i]))
		{
			tokens->push_back(new Token::token(Token::Type::OPERATOR, code[i]));
			++wrapper->pos;
			continue;
		}

		throw std::runtime_error(std::format("invalid char {} at position {}", code[i], i));
		break;
	}

	return tokens;
}

Token::token* Tokenizer::readIdentifier() const
{
	CodeWrapper* wrapper = this->wrapper;
	int i = wrapper->pos;
	const long length = wrapper->length;
	const char* code = wrapper->code;

	string identifier = "";

	while (i < length && Token::isIdentifierValid(code[i]))
		identifier += code[i++];

	wrapper->pos = i;

	return new Token::token(
		Token::Keyword::isKeyword(identifier) ? Token::Type::KEYWORD : Token::Type::STRING,
		identifier
	);
}

Token::token* Tokenizer::readNumber() const
{
	CodeWrapper* wrapper = this->wrapper;
	int i = wrapper->pos;
	const long length = wrapper->length;
	const char* code = wrapper->code;

	string content = "";

	while (i < length && Token::isNumber(code[i]))
		content += code[i++];

	wrapper->pos = i;

	return new Token::token(Token::Type::NUMBER, content);
}