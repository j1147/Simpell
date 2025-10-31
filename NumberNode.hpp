#pragma once
#include "GenericNode.hpp"
#include <string>
#include <iostream>

using std::string;

class NumberNode : public GenericNode
{
public:
	// Converted to numeric values when the program is running
	string preValue;
	double value;
	// Rationale: numbers with decimals (like 1500.25) are merged into one during expression reading
	// because a period is treated as an operator
	bool wasMerged;

	NumberNode(const string& preValue) : preValue(preValue), value(0), wasMerged(false) {};
	NumberNode(double value) : value(value), wasMerged(true) {};

	void log() { std::cout << "Number " << preValue; };
};