#pragma once
#include "CodeWrapper.hpp"
#include "Token.hpp"

CodeWrapper::~CodeWrapper()
{
	delete[] this->code;
}

void CodeWrapper::skipWhitespaces()
{
	int& i = this->pos;
	const long length = this->length;
	const char* code = this->code;
	while (i < length)
		if (Token::isWhitespace(code[i]))
			++i;
		else if (i < length - 1 && code[i] == '/' && code[i + 1] == '/')
			// Skip comments
			while (i < length && !Token::isNewline(code[i]))
				++i;
		else
			break;
}