#pragma once
#include <string>

using std::string;

class Variable
{
public:
	string name;
	int int_value;
	double double_value;
	string string_value;

	Variable(const string& name) : name(name), int_value(0), double_value(0), string_value("") {};
};