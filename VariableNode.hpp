#pragma once
#include "GenericNode.hpp"
#include <string>
#include <iostream>

using std::string;

class VariableNode : public GenericNode
{
public:
	string variableName;

	VariableNode(const string& variableName) : variableName(variableName) {};

	void log() { std::cout << "Variable " << variableName; };
};