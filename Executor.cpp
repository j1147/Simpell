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
#include <random>

#include <iostream>

using std::vector;
using std::string;
using std::runtime_error;
using std::format;

unsigned int Executor::STACK_DEPTH = 0;

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

string Executor::prettyPrintDouble(const double value)
{
	string result = std::to_string(value);
	if (result.find('.') == std::string::npos)
		return result;
	// Ex: 33.00 => 33, 4500.00 => 4500
	if (result.find_last_not_of('0') == result.find('.'))
		result.erase(result.find_last_not_of('0'), std::string::npos);
	// Ex: 12.30400 => 12.304
	else if (result.find_last_not_of('0') > result.find('.'))
		result.erase(result.find_last_not_of('0') + 1, std::string::npos);

	return result;
}

void Executor::execute() const
{
	std::unique_ptr<vector<const Value*>> programInput = std::make_unique<vector<const Value*>>();
#ifdef DEBUG_OUTPUT
	std::cout << "Routines: " << this->routines->size() << "\n";
#endif

	Executor::STACK_DEPTH = 0;

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

Value* Executor::callRoutine(const string& name, const vector<const Value*>* args) const
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
	else if (name == "random")
	{
		if (args->size() != 0)
			throw runtime_error(format("expected 0 arguments for random, but got {}", args->size()));

		std::random_device device;
		std::mt19937 rng(device());

		std::uniform_int_distribution<int> distribution(0, 0x7fff'ffff);

		return new Value(distribution(rng)/(double) 0x7fff'ffff);
	}

	// Find and execute function
	for (const Routine* routine : *this->routines)
	{
		if (routine->name == name)
		{
			// Initialize variables
#ifdef DEBUG_OUTPUT
			std::cout << "Starting execution of " << name << " with " << args->size() << " arguments\n";
#endif
			++Executor::STACK_DEPTH;
			if (Executor::STACK_DEPTH > Executor::MAX_CALL_STACK_SIZE)
				throw runtime_error(format("stack overflow occured trying to run {}", name));

			std::unique_ptr<RoutineContext> context(new RoutineContext(this, routine));
			if (args->size() != routine->parameters)
				throw runtime_error(format("expected {} arguments for {}, but got {}", routine->parameters, name, args->size()));

			for (int v = 0; v < args->size(); ++v)
				context->values[v] = (*args)[v]->clone();

			BlockExecutor executor(routine);
			executor.execute(context.get());
			--Executor::STACK_DEPTH;
			return context->releaseReturnValue();
		}
	}

	throw runtime_error(format("could not find routine {}", name));
}



/*



Executor:
creates a RoutineContext, which contains a vector of Values
RoutineContext->values = vector<Value*>

arg list then clones itself into values
RoutineContext->values = args.clone()


BlockExecutor:

modifies returnValue of RoutineContext
deletes results of ExpressionExecutor

ExpressionExecutor:


*/