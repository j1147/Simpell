#pragma once
#include "Routine.hpp"
#include "Variable.hpp"
#include <vector>

Routine::~Routine()
{
	for (Variable*& v : *variables)
		delete v;
	delete variables;
}