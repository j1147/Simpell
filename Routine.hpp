#pragma once
#include "BlockNode.hpp"
#include "Variable.hpp"
#include <vector>
#include <string>

using std::vector;
using std::string;

class Routine : public BlockNode
{
public:
	string name;
	vector<Variable*>* variables;

	Routine(const string& name, vector<Variable*>* variables) :
		BlockNode(),
		name(name),
		variables(variables)
	{};
	~Routine();
};