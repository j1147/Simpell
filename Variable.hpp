#pragma once
#include <string>

using std::string;

class Variable
{
public:
	string name;

	Variable(const string& name) : name(name) {};
};