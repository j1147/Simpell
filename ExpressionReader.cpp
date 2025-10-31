#pragma once
#include "Token.hpp"
#include "ExpressionReader.hpp"
#include "ExpressionNode.hpp"
#include "FunctionCallNode.hpp"
#include "OperatorNode.hpp"
#include "GenericNode.hpp"
#include "VariableNode.hpp"
#include "NumberNode.hpp"
#include <stdexcept>
#include <format>
#include <vector>
#include <iostream>

using std::runtime_error;
using std::format;
using std::vector;

ExpressionNode* ExpressionReader::parseExpression(ExpressionReader::Context context)
{
	const vector<token*>& tokens = *this->parser->tokens;
	Token::token* token;
	const size_t length = tokens.size();
	int& t = this->parser->pos;

	OperatorNode* currentOperator = nullptr,
		* tempOp = nullptr;
	GenericNode* currentTerm = nullptr;

	int terms = 0;

	unsigned int localPrecedence = 0,
		parentheses = 0;
	
	while (t < length)
	{
		token = tokens[t];
		/*
		Expression contexts:

		var = expr;		=> INLINE
		if expr: ...	=> CONDITION
		var = call func with expr1, expr2, expr3;	=> FUNCTION_CALLs inside INLINE
		if call func with expr1, expr2, expr3: ...	=> FUNCTION_CALLs inside CONDITION
		*/
		if (token->type == Token::Type::CONTROL_FLOW)
			if ((context == ExpressionReader::Context::INLINE || context == ExpressionReader::Context::FUNCTION_ARGUMENT)
				&& token->first() == Token::semicolon)
			{
				++t;
				break;
			}
			else if ((context == ExpressionReader::Context::CONDITION || context == ExpressionReader::Context::FUNCTION_ARGUMENT)
				&& token->first() == Token::colon)
			{
				++t;
				break;
			}
			else if (context == ExpressionReader::Context::FUNCTION_ARGUMENT && token->first() == Token::comma)
			{
				++t;
				break;
			}
			else throw runtime_error(format("unexpected token {}, delete this", token->content));

		// Unfortunately ad-hoc true/false conversion
		if (token->type == Token::Type::KEYWORD && (token->content == Token::Keyword::kw_true || token->content == Token::Keyword::kw_false))
		{
			token->type = Token::Type::NUMBER;
			token->content = token->content == Token::Keyword::kw_true ? "1" : "0";
		}

		// Tree building
		if (token->type == Token::Type::OPERATOR)
		{
			// Parentheses
			if (token->first() == Token::Operator::l_parenthesis)
			{
				localPrecedence += Token::Operator::HIGHEST_PRECEDENCE;
				++parentheses;
				++t;
				continue;
			}
			else if (token->first() == Token::Operator::r_parenthesis)
			{
				if (parentheses == 0)
					throw runtime_error("unbalanced parentheses");
				localPrecedence -= Token::Operator::HIGHEST_PRECEDENCE;
				--parentheses;
				++t;
				continue;
			}

			// Operators
			if (currentOperator == nullptr)
			{
				// Nothing to operate on yet
				if (currentTerm == nullptr && !Token::Operator::isUnaryOperator(token->first()))
					throw runtime_error(format("unexpected token {}, delete this", token->content));

				currentOperator = new OperatorNode(
					token->first(),
					Token::Operator::getPrecedence(token->first()) + localPrecedence
				);

				if (currentTerm != nullptr)
				{
					// ! must come before, not after
					if (currentOperator->isUnary)
						throw runtime_error(format("unexpected token {}, delete this", token->content));
					currentOperator->consume(currentTerm);
					currentTerm = nullptr;
				}

				++terms;
				++t;
				continue;
			}

			// Tree action
			// Avoid cases like "+ *", but accept cases like "!!" (need a better system for this)
			if (!currentOperator->isFull())
			{
				if (currentOperator->isUnary)
					if (Token::Operator::isUnaryOperator(token->first()))
					{
						// Ex.: !!
						tempOp = new OperatorNode(
							token->first(),
							Token::Operator::getPrecedence(token->first()) + localPrecedence
						);
						currentOperator->consume(tempOp);
						currentOperator = tempOp;
					}
					else
						// Ex.: !+
						throw runtime_error(format("unexpected token {}, delete this", token->content));
				else if (Token::Operator::isUnaryOperator(token->first()))
				{
					// Ex.: a+!
					tempOp = new OperatorNode(
						token->first(),
						Token::Operator::getPrecedence(token->first()) + localPrecedence
					);
					currentOperator->consume(tempOp);
					currentOperator = tempOp;
				}
				else
					// Ex.: a+*
					throw runtime_error(format("unexpected token {}, delete this", token->content));
			}
			else
			{
				// Ex(s).: a+b* , !a-
				tempOp = new OperatorNode(
					token->first(),
					Token::Operator::getPrecedence(token->first()) + localPrecedence
				);
				while (currentOperator->parent != nullptr && currentOperator->localPrecedence > tempOp->localPrecedence)
					currentOperator = currentOperator->parent;

				// Somewhere in the tree
				// Consuming takes care of replacing links
				if (currentOperator->localPrecedence < tempOp->localPrecedence)
					// Going down decreases precedence, take nodes from lower precedence
					tempOp->consume(currentOperator->getRightmostNode());
				else
					tempOp->consume(currentOperator);
				currentOperator = tempOp;
			}

			++t;
			++terms;
		}
		else if (token->type == Token::Type::STRING || token->type == Token::Type::NUMBER)
		{
			// Numbers & variables
			if ((currentOperator == nullptr && currentTerm != nullptr)
				|| (currentOperator != nullptr && currentOperator->isFull()))
				throw runtime_error(format("unexpected token {}, delete this", token->content));

			currentTerm = token->type == Token::Type::STRING ?
				(GenericNode*) new VariableNode(token->content)
				:
				(GenericNode*) new NumberNode(token->content);

			if (currentOperator)
			{
				currentOperator->consume(currentTerm);
				currentTerm = nullptr;
			}

			++t;
			++terms;
		}
		else if (token->type == Token::Type::KEYWORD)
		{
			// Function calls
			if (token->content == Token::Keyword::kw_call)
			{
				if (terms > 0 || context == ExpressionReader::Context::FUNCTION_ARGUMENT)
					throw runtime_error(format("function calls cannot be nested"));
				// Function calls should be on their own
				ExpressionReader callReader(this->parser);
				return new ExpressionNode(callReader.parseFunctionCall(context));
			}
			else
				// Statements and blocks delegated elsewhere
				throw runtime_error(format("unexpected keyword {}, delete this", token->content));
		}
	}
	if (!currentOperator)
	{
		return nullptr;
	}
	// Get to the top of the tree
	while (currentOperator->parent)
		currentOperator = currentOperator->parent;

	currentOperator->log();
	std::cout << '\n';

	return nullptr;
}

FunctionCallNode* ExpressionReader::parseFunctionCall(ExpressionReader::Context context)
{
	return nullptr;
}