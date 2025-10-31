#pragma once
#include "Token.hpp"
#include "SyntaxParser.hpp"
#include "Routine.hpp"
#include "Variable.hpp"
#include "ExpressionReader.hpp"
#include <vector>
#include <stdexcept>
#include <format>
#include <string>

#include <iostream>

using std::vector;
using std::runtime_error;
using std::format;
using std::string;

vector<Routine*>* SyntaxParser::parse()
{
	vector<Routine*>* routines = new vector<Routine*>();
	
	const size_t length = tokens->size();
	Token::token* token;

	while (this->pos < length)
	{
		token = (*tokens)[this->pos];
		if (token->type != Token::Type::KEYWORD)
			throw runtime_error(format("unexpected token {}, delete this", token->content));

		if (token->content == Token::Keyword::kw_routine)
			routines->push_back(parseRoutine());
		else
			throw runtime_error(format("expected a routine definition, got {} instead", token->content));
	}

	return routines;
}

Routine* SyntaxParser::parseRoutine()
{
	Routine* routine = parseRoutineHeader();
	
	parseVariableDefinitions(routine);

	delete routine;

	return nullptr;
}

Routine* SyntaxParser::parseRoutineHeader()
{
	const vector<Token::token*>& tokens = *this->tokens;
	Token::token* token;
	const size_t length = tokens.size();
	int& t = this->pos;

	// Name
	++t;
	if (t >= length)
		throw runtime_error("unexpected end of file, expected routine name");

	token = tokens[t];
	if (token->type != Token::Type::STRING)
		throw runtime_error(format("expected a routine name, got {} instead", token->content));

	string name = token->content;
	std::cout << "Routine name is " << name << '\n';

	// Using keyword
	++t;
	if (t >= length)
		throw runtime_error(format("incomplete routine header for {}, missing using keyword", name));

	token = tokens[t];
	if (token->type != Token::Type::KEYWORD || token->content != Token::Keyword::kw_using)
		throw runtime_error(format("incomplete routine header for {}, missing using keyword", name));

	// Parameter list
	++t;
	if (t >= length)
		throw runtime_error(format("incomplete parameter list for {}", name));

	token = tokens[t];
	if (token->type != Token::Type::STRING && token->content != Token::Keyword::kw_nothing)
		throw runtime_error(format("invalid parameter '{}' for {}", token->content, name));

	vector<Variable*>* variables = new vector<Variable*>();
	++t;
	if (t >= length)
		throw runtime_error(format("expected : after parameter list for {}", name));

	if (token->content == Token::Keyword::kw_nothing)
	{
		// No parameters (nothing)
		token = tokens[t];
		if (token->type != Token::Type::CONTROL_FLOW || token->first() != Token::colon)
			throw runtime_error(format("expected : after parameter list for {}", name));
		++t;
	}
	else
	{
		variables->push_back(new Variable(token->content));
		// List of parameters
		while (t < length)
		{
			// , or :
			token = tokens[t];
			if (token->type != Token::Type::CONTROL_FLOW)
				throw runtime_error(format("expected , or : but got {} in parameter list for {}", token->content, name));
			if (token->first() == Token::colon)
			{
				++t;
				goto finishedParameters;
			}

			if (token->first() != Token::comma)
				throw runtime_error(format("expected , but got {} in parameter list for {}", token->content, name));

			++t;
			if (t >= length)
				throw runtime_error(format("expected parameter name after , in parameter list for {}", name));

			// Parameter name
			token = tokens[t];
			if (token->type != Token::Type::STRING)
				throw runtime_error(format("invalid parameter name '{}' in parameter list for {}", token->content, name));
			variables->push_back(new Variable(token->content));
			++t;
		}
		throw runtime_error(format("unterminated parameter list for {}", name));
	}

	finishedParameters:
	std::cout << "Got parameters.\n";
	for (Variable*& var : *variables)
		std::cout << var->name << "\n";

	return new Routine(name, variables);
}

void SyntaxParser::parseVariableDefinitions(Routine* routine)
{
	const vector<Token::token*>& tokens = *this->tokens;
	Token::token* token;
	const size_t length = tokens.size();
	int& t = this->pos;

	while (t < length)
	{
		token = tokens[t];
		if (token->type != Token::Type::KEYWORD || token->content != Token::Keyword::kw_define)
			break;

		// Variable name
		++t;
		if (t >= length)
			throw runtime_error(format("expected variable name"));

		token = tokens[t];
		if (token->type != Token::Type::STRING)
			throw runtime_error(format("invalid variable name {}", token->content));
		string name = token->content;

		std::cout << format("Name is {}\n", name);

		// Variable assignment
		++t;
		if (t >= length)
			throw runtime_error(format("unexpected end of variable definition"));

		token = tokens[t];

		if (token->type == Token::Type::CONTROL_FLOW && token->first() == Token::semicolon)
		{
			// No definition
			++t;
			continue;
		}
		else if (token->type == Token::Type::OPERATOR && token->first() == Token::Operator::equal)
		{
			// Expression reader
			// Step back so the expression reader can read the variable name
			--t;
			ExpressionReader expressionReader(this);
			routine->subtrees->push_back(expressionReader.parseExpression(ExpressionReader::Context::INLINE));
			continue;
		}
		else throw runtime_error(format("unexpected token {} in variable definition, delete this", token->content));
	}
}