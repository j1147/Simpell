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
	int pos;

	SyntaxParser(vector<token*>* tokens) : tokens(tokens), pos(0) {};
	vector<Routine*>* parse();
	Routine* parseRoutine();
	Routine* parseRoutineHeader();
	void parseVariableDefinitions(Routine* routine);
};