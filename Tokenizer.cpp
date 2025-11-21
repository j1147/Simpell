#pragma once
#include "_global.hpp"
#include "Tokenizer.hpp"
#include "CodeWrapper.hpp"
#include "Token.hpp"
#include <vector>
#include <string>
#include <stdexcept>
#include <format>
#include <memory>
#include <iostream>

using std::vector;
using std::string;
using std::format;
using std::runtime_error;

vector<Token::token*>* Tokenizer::scan() const
{
	CodeWrapper* wrapper = this->wrapper;
	unsigned int& i = wrapper->pos;
	const size_t length = wrapper->length;
	const char* code = wrapper->code;

	std::unique_ptr<vector<Token::token*>> tokens = std::make_unique<vector<Token::token*>>();

	while (i < length)
	{
		wrapper->skipWhitespaces();
		if (i >= length)
			break;

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

		if (code[i] == Token::quote)
		{
			tokens->push_back(readStringLiteral());
			continue;
		}

		if (Token::isControlFlow(code[i]))
		{
			tokens->push_back(new Token::token(
				Token::Type::CONTROL_FLOW,
				code[i],
				wrapper->lineNumber
			));
			++i;
			continue;
		}

		if (Token::Operator::isOperator(code[i]))
		{
			tokens->push_back(new Token::token(
				Token::Type::OPERATOR,
				code[i],
				wrapper->lineNumber
			));
			++i;
			continue;
		}

		deleteAll(tokens.get());
		throw runtime_error(format("invalid char {} at position {}", code[i], i));
	}

	return tokens.release();
}

Token::token* Tokenizer::readIdentifier() const
{
	CodeWrapper* wrapper = this->wrapper;
	unsigned int& i = wrapper->pos;
	const size_t length = wrapper->length;
	const char* code = wrapper->code;

	string identifier = "";

	while (i < length && Token::isIdentifierValid(code[i]))
		identifier += code[i++];

	return new Token::token(
		Token::Keyword::isKeyword(identifier) ? Token::Type::KEYWORD : Token::Type::NAME,
		identifier,
		wrapper->lineNumber
	);
}

Token::token* Tokenizer::readNumber() const
{
	CodeWrapper* wrapper = this->wrapper;
	unsigned int& i = wrapper->pos;
	const size_t length = wrapper->length;
	const char* code = wrapper->code;

	string content = "";

	int period_count = 0;

	while (i < length && (
		Token::isNumber(code[i])
		||
		(code[i] == Token::Operator::dot.first() && period_count++ == 0)
	))
		content += code[i++];

	return new Token::token(Token::Type::NUMBER, content, wrapper->lineNumber);
}

Token::token* Tokenizer::readStringLiteral() const
{
	CodeWrapper* wrapper = this->wrapper;
	unsigned int& i = wrapper->pos;
	const size_t length = wrapper->length;
	const char* code = wrapper->code;

	string content = "";

	++i;
	while (i < length && code[i] != Token::quote)
	{
		if (Token::isNewline(code[i]))
			throw runtime_error(format("invalid character in string literal on line {}", wrapper->lineNumber));
		content += code[i++];
	}

	if (i >= length)
		throw runtime_error("unterminated string literal");
	++i;

	return new Token::token(Token::Type::STRING_LITERAL, content, wrapper->lineNumber);
}