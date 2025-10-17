#pragma once
#include "CodeWrapper.hpp"
#include "Token.hpp"
#include <vector>

class Tokenizer
{
public:
	CodeWrapper* wrapper;
	Tokenizer(CodeWrapper* wrapper) : wrapper(wrapper) {};
	std::vector<Token::token*>* parse();
	Token::token* readIdentifier();
	Token::token* readNumber();
};