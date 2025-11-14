#pragma once
#include "CodeWrapper.hpp"
#include "Token.hpp"
#include <vector>

class Tokenizer
{
public:
	CodeWrapper* wrapper;
	Tokenizer(CodeWrapper* wrapper) : wrapper(wrapper) {};
	std::vector<Token::token*>* scan() const;
	Token::token* readIdentifier() const;
	Token::token* readNumber() const;
	Token::token* readStringLiteral() const;
};