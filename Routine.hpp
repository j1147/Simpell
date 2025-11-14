#pragma once
#include "BlockNode.hpp"
#include "Variable.hpp"
#include "Token.hpp"
#include <vector>
#include <string>

using std::vector;
using std::string;

class Routine : public BlockNode
{
public:
	const string name;
	const size_t parameters;
	vector<Variable*>* variables;

	Routine(const string& name, const size_t parameters, vector<Variable*>* variables) :
		BlockNode(&Token::Keyword::kw_routine),
		name(name),
		parameters(parameters),
		variables(variables)
	{};
	~Routine();
};