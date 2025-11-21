#pragma once
#include "_global.hpp"
#include "ExpressionExecutor.hpp"
#include "Value.hpp"
#include "AbstractNode.hpp"
#include "RoutineContext.hpp"
#include "Token.hpp"
#include "NumberNode.hpp"
#include "ExpressionReader.hpp"
#include "RoutineCallNode.hpp"
#include "AbstractBranchNode.hpp"
#include "OperatorNode.hpp"
#include <cmath>
#include <vector>
#include <memory>
#include <format>
#include <stdexcept>

#include <iostream>

using std::vector;
using std::runtime_error;
using std::format;

Value* ExpressionExecutor::execute(RoutineContext* routineContext, ExpressionReader::Context context) const
{
	const AbstractNode& top = *this->expression->top;

	const std::unique_ptr<vector<const AbstractNode*>> nodes = std::make_unique<vector<const AbstractNode*>>();
	top.dumpToStack(nodes.get());

	vector<Value*>& variableValues = routineContext->values;

	std::unique_ptr<vector<Value*>> stack = std::make_unique<vector<Value*>>();
	try
	{
		for (const AbstractNode* node : *nodes)
			if (node->token->type == Token::Type::NUMBER)
			{
#ifdef DEBUG_OUTPUT
				std::cout << "Pushing NUMBER: " << dynamic_cast<const NumberNode*>(node)->value << "\n";
#endif
				// Numeric values
				stack->push_back(new Value(
					dynamic_cast<const NumberNode*>(node)->value
				));
			}
			else if (node->token->type == Token::Type::STRING_LITERAL)
			{
#ifdef DEBUG_OUTPUT
				std::cout << "Pushing STRING: " << node->token->content << "\n";
#endif
				// String values
				stack->push_back(new Value(
					new string(node->token->content) // !!!
				));
			}
			else if (node->token->type == Token::Type::NAME)
			{
#ifdef DEBUG_OUTPUT
				std::cout << "Pushing VARIABLE: " << node->token->content << "\n";
#endif
				// Variable accesses
				int v = routineContext->findVariable(node->token->content);
				if (v == -1)
					throw runtime_error(format("undefined variable {} (near line {})", node->token->content, node->token->lineNumber));
				
				// Hacky fix for assignment statements
				if (!variableValues[v] && !(
					node->parent->token->type == Token::Type::OPERATOR
					&& node->parent->token->first() == Token::Operator::equal.first()
					&& node->parent == this->expression->top
					&& context == ExpressionReader::Context::INLINE
				))
					throw runtime_error(format("variable {} has not been declared yet (near line {})", node->token->content, node->token->lineNumber));
				
				// Only allowed to be null when an assignment takes place
				stack->push_back(variableValues[v] ? variableValues[v]->clone() : nullptr);
			}
			else if (node->token->type == Token::Type::KEYWORD && node->token->content == Token::Keyword::kw_call.content)
			{
				// Function calls
				const RoutineCallNode* routineCall = dynamic_cast<const RoutineCallNode*>(node);
#ifdef DEBUG_OUTPUT
				std::cout << "Pushing ROUTINE CALL: " << routineCall->routineName << "\n";
#endif
				std::unique_ptr<vector<const Value*>> args = std::make_unique<vector<const Value*>>();

				try
				{
					for (const ExpressionNode* arg : *routineCall->args)
					{
						ExpressionExecutor executor(arg);
						args->push_back(executor.execute(
							routineContext,
							ExpressionReader::Context::FUNCTION_ARGUMENT
						));
					}
				}
				catch (std::exception error)
				{
					deleteAll(args.get());
					throw error;
				}

				Value* returnValue = routineContext->mainProgram->callRoutine(
					*routineCall->routineName,
					args.get()
				);

				if (!returnValue && !(context == ExpressionReader::Context::INLINE && this->expression->top == node))
					throw runtime_error(format("undefined routine result from {}", *routineCall->routineName));

				stack->push_back(returnValue);

				deleteAll(args.get());
			}
			else if (node->token->type == Token::Type::OPERATOR)
			{
#ifdef DEBUG_OUTPUT
				std::cout << "Going to operate with \n";
#endif
				// And finally, the most important operation: operators
				const char op = node->token->first();
				if (stack->size() < 1)
					throw runtime_error(format("undefined right-hand side for {} (near line {})", op, node->token->lineNumber));

				Value* rhs = stack->back();
				// Does not call destructor. Destroys the pointer, like delete &rhs; not delete rhs;
				stack->pop_back();

				// Unary operators
				if (op == Token::Operator::invert.first())
				{
					// Not (!)
					if (!rhs)
						throw runtime_error(format("undefined right-hand side for {} (near line {})", op, node->token->lineNumber));
					
					if (Executor::isTruthy(rhs))
						stack->push_back(new Value(1));
					else
						stack->push_back(new Value(0.0));
					delete rhs;
#ifdef DEBUG_OUTPUT
					std::cout << "Operator " << op << " consumed 1 term\n";
#endif
					continue;
				}

				// 1 because rhs has already been popped off
				if (stack->size() < 1)
					throw runtime_error(format("undefined left-hand side for {} (near line {})", op, node->token->lineNumber));

				Value* lhs = stack->back();
				stack->pop_back();

				const OperatorNode* operatorNode = dynamic_cast<const OperatorNode*>(node);

				// Binary operators
				// Assignment
				// In this case only, lhs is allowed to be undefined (uninitialized)
				if (op == Token::Operator::equal.first()
					&& context == ExpressionReader::Context::INLINE && this->expression->top == node)
				{
					// Variable is already known to exist, otherwise it wouldn't be on the stack
					int v = routineContext->findVariable(operatorNode->left->token->content);
					assert(v != -1);
					assert(v < routineContext->values.size());

					delete routineContext->values[v];
					routineContext->values[v] = rhs;
					delete lhs;
#ifdef DEBUG_OUTPUT
					std::cout << "Assigned value to " << operatorNode->left->token->content << "\n";
#endif
					continue;
				}

				// All other cases require that the lhs exists
				if (!lhs)
					throw runtime_error(format("undefined left-hand side for {} (near line {})", op, node->token->lineNumber));

				// Other operators
				if (op == Token::Operator::equal.first())
				{
					// Equals
					if (lhs->which != rhs->which)
						stack->push_back(new Value(0.0));
					else if (lhs->which == Value::Which::Double)
						stack->push_back(new Value(
							lhs->getDouble() == rhs->getDouble()
						));
					else if (lhs->which == Value::Which::String)
						stack->push_back(new Value(
							lhs->getString() == rhs->getString()
						));
				}
				else if (op == Token::Operator::add.first())
				{
					// Add
					if (lhs->which == Value::Which::Double)
						if (rhs->which == Value::Which::Double)
							stack->push_back(new Value(
								lhs->getDouble() + rhs->getDouble()
							));
						else
							stack->push_back(new Value(new string(
								Executor::prettyPrintDouble(lhs->getDouble()) + *rhs->getString()
							)));
					else
						if (rhs->which == Value::Which::Double)
							stack->push_back(new Value(new string(
								*lhs->getString() + Executor::prettyPrintDouble(rhs->getDouble())
							)));
						else
							stack->push_back(new Value(new string(
								*lhs->getString() + *rhs->getString()
							)));
				}
				else if (op == Token::Operator::sub.first()
					|| op == Token::Operator::mul.first()
					|| op == Token::Operator::div.first()
					|| op == Token::Operator::mod.first()
					|| op == Token::Operator::less_than.first()
					|| op == Token::Operator::greater_than.first())
				{
					if (lhs->which != Value::Which::Double || rhs->which != Value::Which::Double)
					{
						delete lhs;
						delete rhs;
						throw runtime_error(format("attempting to perform math on a string (near line {})", node->token->lineNumber));
					}
					stack->push_back(mathOp(op, lhs, rhs));
				}
				else if (op == Token::Operator::op_and.first())
					// And
					stack->push_back(new Value(
						Executor::isTruthy(lhs) && Executor::isTruthy(rhs)
					));
				else if (op == Token::Operator::op_or.first())
					// Or
					stack->push_back(new Value(
						Executor::isTruthy(lhs) || Executor::isTruthy(rhs)
					));
#ifdef DEBUG_OUTPUT
				std::cout << "Operator " << op << " consumed 2 terms\n";
#endif

				delete lhs;
				delete rhs;
			}
	}
	catch (std::exception error)
	{
		deleteAll(stack.get());
		throw error;
	}

#ifdef DEBUG_OUTPUT
	std::cout << "Finished expression.\n";
#endif

	if (stack->size() == 0)
		return nullptr;
	assert(stack->size == 1);

	Value* result = stack->back();

	stack->clear();
	return result;
}

Value* ExpressionExecutor::mathOp(char op, const Value* lhs, const Value* rhs) const
{
	double L = lhs->getDouble(),
		R = rhs->getDouble();

	if (op == Token::Operator::sub.first())
		return new Value(L - R);
	else if (op == Token::Operator::mul.first())
		return new Value(L * R);
	else if (op == Token::Operator::div.first())
		return new Value(L / R);
	else if (op == Token::Operator::mod.first())
		return new Value(std::fmod(L, R));
	else if (op == Token::Operator::less_than.first())
		return new Value(L < R);
	else if (op == Token::Operator::greater_than.first())
		return new Value(L > R);

	return nullptr;
}