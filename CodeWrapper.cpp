#pragma once
#include "_global.hpp"
#include "CodeWrapper.hpp"
#include "Token.hpp"

CodeWrapper::~CodeWrapper()
{
	delete[] this->code;
}

void CodeWrapper::skipWhitespaces()
{
	unsigned int& i = this->pos;
	const size_t length = this->length;
	const char* code = this->code;
	while (i < length)
	{
		if (code[i] == Token::newline)
			++this->lineNumber;
		if (Token::isWhitespace(code[i]))
			++i;
		else if (i < length - 1 && code[i] == '/' && code[i + 1] == '/')
			// Skip comments
			while (i < length && !Token::isNewline(code[i]))
				++i;
		else
			break;
	}
}