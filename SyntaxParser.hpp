#pragma once
#include <vector>
#include "Token.hpp"
#include "GenericNode.hpp"
#include "Routine.hpp"

using std::vector;
using Token::token;

class SyntaxParser
{
public:
	vector<token*>* tokens;

	SyntaxParser(vector<token*>* tokens) : tokens(tokens) {};
	vector<Routine*>* parse();
};