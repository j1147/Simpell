#pragma once
#include "_global.hpp"
#include "Token.hpp"
#include "ExpressionReader.hpp"
#include "ExpressionNode.hpp"
#include "RoutineCallNode.hpp"
#include "OperatorNode.hpp"
#include "AbstractNode.hpp"
#include "AbstractBranchNode.hpp"
#include "VariableNode.hpp"
#include "NumberNode.hpp"
#include "MetaParser.hpp"
#include "StringNode.hpp"
#include <stdexcept>
#include <format>
#include <vector>
#include <iostream>

using std::runtime_error;
using std::format;
using std::vector;

ExpressionNode* ExpressionReader::parseExpression(ExpressionReader::Context context)
{
	const vector<Token::token*>& tokens = *this->parser->tokens;
	Token::token* token = nullptr;
	const size_t length = tokens.size();
	int& t = this->parser->pos;

	AbstractBranchNode* currentOperator = nullptr,
		* tempOp = nullptr;
	AbstractNode* currentTerm = nullptr;

	int terms = 0;

	unsigned int localPrecedence = 0,
		parentheses = 0;
	
	try
	{
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
					break;
				else if ((context == ExpressionReader::Context::CONDITION || context == ExpressionReader::Context::FUNCTION_ARGUMENT)
					&& token->first() == Token::colon)
					break;
				else if (context == ExpressionReader::Context::FUNCTION_ARGUMENT && token->first() == Token::comma)
					break;
				else
					throw runtime_error(this->parser->format("unexpected token {}, delete this", token->content));

			// Unfortunately ad-hoc true/false conversion
			if (token->type == Token::Type::KEYWORD &&
				(token->content == Token::Keyword::kw_true.content || token->content == Token::Keyword::kw_false.content))
			{
				token->type = Token::Type::NUMBER;
				token->content = token->content == Token::Keyword::kw_true.content ? "1" : "0";
			}

			// Tree building
			if (token->type == Token::Type::OPERATOR)
			{
				// Parentheses
				if (token->first() == Token::Operator::l_parenthesis.first())
				{
					localPrecedence += Token::Operator::HIGHEST_PRECEDENCE;
					++parentheses;
					++t;
					continue;
				}
				else if (token->first() == Token::Operator::r_parenthesis.first())
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
						throw runtime_error(this->parser->format("unexpected token {}, delete this", token->content));

					currentOperator = new OperatorNode(
						//Token::Operator::getOperator(token->first()),
						token,
						// Precedence of equal (=) changes depending on context (used as both assignment + equality)
						(token->first() == Token::Operator::equal.first() && 
							(context == ExpressionReader::Context::CONDITION || context == ExpressionReader::Context::FUNCTION_ARGUMENT) ?
							Token::Operator::getPrecedence(Token::Operator::greater_than.first())
							:
							Token::Operator::getPrecedence(token->first())) + localPrecedence
					);

					if (currentTerm != nullptr)
					{
						// ! must come before, not after
						if (currentOperator->isUnary)
							throw runtime_error(this->parser->format("unexpected token {}, delete this", token->content));
						currentOperator->consume(currentTerm);
						currentTerm = nullptr;
					}

					++terms;
					++t;
					continue;
				}

				// Tree building
				// Avoid cases like "+ *", but accept cases like "!!" (need a better system for this)
				if (!currentOperator->isFull())
				{
					if (currentOperator->isUnary)
						if (Token::Operator::isUnaryOperator(token->first()))
						{
							// Ex.: !!
							tempOp = new OperatorNode(
								//Token::Operator::getOperator(token->first()),
								token,
								Token::Operator::getPrecedence(token->first()) + localPrecedence
							);
							currentOperator->consume(tempOp);
							currentOperator = tempOp;
							tempOp = nullptr;
						}
						else
							// Ex.: !+
							throw runtime_error(this->parser->format("unexpected token {}, delete this", token->content));
					else if (Token::Operator::isUnaryOperator(token->first()))
					{
						// Ex.: a+!
						tempOp = new OperatorNode(
							//Token::Operator::getOperator(token->first()),
							token,
							Token::Operator::getPrecedence(token->first()) + localPrecedence
						);
						currentOperator->consume(tempOp);
						currentOperator = tempOp;
						tempOp = nullptr;
					}
					else
						// Ex.: a+*
						throw runtime_error(this->parser->format("unexpected token {}, delete this", token->content));
				}
				else
				{
					// Ex(s).: a+b* , !a-
					tempOp = new OperatorNode(
						//Token::Operator::getOperator(token->first()),
						token,
						// Precedence of equal (=) changes depending on context (used as both assignment + equality)
						(token->first() == Token::Operator::equal.first() && terms > 2 ?
							Token::Operator::getPrecedence(Token::Operator::greater_than.first())
							:
							Token::Operator::getPrecedence(token->first())) + localPrecedence
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
					tempOp = nullptr;
				}

				++t;
				++terms;
			}
			else if (token->type == Token::Type::NAME || token->type == Token::Type::NUMBER || token->type == Token::Type::STRING_LITERAL)
			{
				// Numbers & variables
				if ((currentOperator == nullptr && currentTerm != nullptr)
					|| (currentOperator != nullptr && currentOperator->isFull()))
					throw runtime_error(this->parser->format("unexpected token {}, delete this", token->content));

				currentTerm = token->type == Token::Type::NAME ?
					(AbstractNode*) new VariableNode(token)
					:
					token->type == Token::Type::STRING_LITERAL ?
					(AbstractNode*) new StringNode(token)
					:
					(AbstractNode*) new NumberNode(token);

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
				if (token->content == Token::Keyword::kw_call.content)
				{
					if ((currentOperator == nullptr && currentTerm != nullptr)
						|| (currentOperator != nullptr && currentOperator->isFull()))
						throw runtime_error(this->parser->format("unexpected token {}, delete this", token->content));

					if (terms > 2 || context == ExpressionReader::Context::FUNCTION_ARGUMENT)
						throw runtime_error(this->parser->format("function calls cannot be nested"));
					// Function calls should be on their own, either as an assignment or as a call
					currentTerm = parseFunctionCall(context);

					if (currentOperator)
					{
						currentOperator->consume(currentTerm);
						currentTerm = nullptr;
					}

					++terms;
					// Should immediately break afterwards, due to a function call's consuming nature
					--t; // Side effect: prevent issues when function call is the last thing in a file
					break;
				}
				else
					// Statements and blocks delegated elsewhere
					throw runtime_error(this->parser->format("unexpected keyword {}, delete this", token->content));
			}
		}

		if (t >= length)
			throw runtime_error(this->parser->format("unexpected end of file"));
		// Move past colon/semicolon
		++t;

		if (!currentOperator)
		{
			if (currentTerm)
				return new ExpressionNode(currentTerm);
			return nullptr;
		}

		if (!currentOperator->isFull())
			throw runtime_error(this->parser->format("unexpected end of expression"));

		// Get to the top of the tree
		while (currentOperator->parent)
			currentOperator = currentOperator->parent;
	}
	catch (std::exception error)
	{
		if (currentTerm)
			if (currentOperator)
				delete currentOperator;
			else
				delete currentTerm;
		else if (currentOperator)
			delete currentOperator;

		if (tempOp)
			delete tempOp;
		throw error;
	}


	return new ExpressionNode(currentOperator);
}

RoutineCallNode* ExpressionReader::parseFunctionCall(ExpressionReader::Context context)
{
	MetaParser parser(this->parser);

	parser.verifyKeyword(Token::Keyword::kw_call);
	string* routineName = parser.getString();
	parser.verifyKeyword(Token::Keyword::kw_with);

	if (parser.softVerifyKeyword(Token::Keyword::kw_nothing))
	{
		if (!parser.softVerifyTerminator(context))
			throw new runtime_error(format("expected end of function call after 'nothing' keyword"));
		return new RoutineCallNode(routineName, new vector<ExpressionNode*>(0));
	}

	vector<ExpressionNode*>* args = parser.parseExpressionList(context);

	return new RoutineCallNode(routineName, args);
}