#pragma once
#include "_global.hpp"
#include "Executor.hpp"
#include "Routine.hpp"
#include "RoutineContext.hpp"
#include "Value.hpp"
#include "BlockExecutor.hpp"
#include <vector>
#include <string>
#include <stdexcept>
#include <format>
#include <memory>

#include <iostream>

using std::vector;
using std::string;
using std::runtime_error;
using std::format;

bool Executor::isTruthy(const Value* value)
{
	// JavaScript-like if statements
		// If the string does not exist, is "", or the numerical value is 0, the condition is not satisfied
	if (!value)
		return false;
	if (value->which == Value::Which::String
		&& (value->getString() == nullptr || *value->getString() == ""))
		return false;
	else if (value->which == Value::Which::Double && value->getDouble() == 0.0)
		return false;

	return true;
}

void Executor::execute() const
{
	std::unique_ptr<vector<const Value*>> programInput = std::make_unique<vector<const Value*>>();
#ifdef DEBUG_OUTPUT
	std::cout << "Routines: " << this->routines->size() << "\n";
#endif

	try
	{
		Value* programOutput = callRoutine("main", programInput.get());

		delete programOutput;
	}
	catch (std::exception error)
	{
		deleteAll(programInput.get());
		throw error;
	}
}

Value* Executor::callRoutine(const string& name, vector<const Value*>* args) const
{
	// Special functions
	if (name == "print")
	{
		const size_t length = args->size();
		const vector<const Value*>& values = *args;
		if (length > 0)
			if (values[0]->which == Value::Which::Double)
				std::cout << values[0]->getDouble();
			else
				std::cout << *values[0]->getString();

		for (int a = 1; a < length; ++a)
			if (values[a]->which == Value::Which::Double)
				std::cout << " " << values[a]->getDouble();
			else
				std::cout << " " << *values[a]->getString();
		std::cout << "\n";
		return nullptr;
	}

	for (const Routine* routine : *this->routines)
	{
		if (routine->name == name)
		{
			// Initialize variables
#ifdef DEBUG_OUTPUT
			std::cout << "Starting execution of " << name << " with " << args->size() << " arguments\n";
#endif
			std::unique_ptr<RoutineContext> context(new RoutineContext(this, routine));
			if (args->size() != routine->parameters)
				throw runtime_error(format("expected {} arguments for {}, but got {}", routine->parameters, name, args->size()));

			for (int v = 0; v < args->size(); ++v)
				context->values[v] = (*args)[v]->clone();

			BlockExecutor executor(routine);
			executor.execute(context.get());
			return context->returnValue;
		}
	}

	throw runtime_error(format("could not find routine {}", name));
}