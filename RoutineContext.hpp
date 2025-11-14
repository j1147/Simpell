#pragma once
#include "Value.hpp"
#include "Routine.hpp"
#include "Executor.hpp"
#include <vector>
#include <string>

using std::vector;
using std::string;

class RoutineContext
{
public:
	const Executor* mainProgram;
	const Routine* parent;
	vector<Value*> values;
	Value* returnValue;

	RoutineContext(const Executor* mainProgram, const Routine* parent) :
		mainProgram(mainProgram),
		parent(parent),
		values(vector<Value*>(parent->variables->size(), nullptr)),
		returnValue(nullptr)
	{};

	int findVariable(const string& name) const;
	~RoutineContext();
};