#pragma once
#include "_global.hpp"
#include "RoutineContext.hpp"
#include "Routine.hpp"
#include <string>
#include <vector>

using std::string;
using std::vector;

int RoutineContext::findVariable(const string& name) const
{
	const size_t length = parent->variables->size();
	const vector<Variable*>& variables = *(parent->variables);

	for (int v = 0; v < length; ++v)
		if (variables[v]->name == name)
			return v;

	return -1;
}

RoutineContext::~RoutineContext()
{
	values.clear();
	delete returnValue;
}