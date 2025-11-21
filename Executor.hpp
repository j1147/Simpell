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
	static unsigned int STACK_DEPTH;
	static constexpr unsigned int MAX_CALL_STACK_SIZE = 48;
	static bool isTruthy(const Value* value);
	static string prettyPrintDouble(const double value);
	const vector<Routine*>* routines;

	Executor(const vector<Routine*>* routines) : routines(routines) {};

	void execute() const;
	Value* callRoutine(const string& name, const vector<const Value*>* params) const;
};