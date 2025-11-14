#pragma once
#include "Routine.hpp"
#include "Value.hpp"
#include <vector>
#include <string>

using std::vector;
using std::string;

class Executor
{
public:
	static bool isTruthy(const Value* value);
	const vector<Routine*>* routines;

	Executor(const vector<Routine*>* routines) : routines(routines) {};

	void execute() const;
	Value* callRoutine(const string& name, vector<const Value*>* params) const;
};