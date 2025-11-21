#pragma once
#include "_global.hpp"
#include "Token.hpp"
#include "SyntaxParser.hpp"
#include "Routine.hpp"
#include "Variable.hpp"
#include "ExpressionReader.hpp"
#include "MetaParser.hpp"
#include <vector>
#include <stdexcept>
#include <string>
#include <memory>
#include <format>
#include <iostream>

using std::vector;
using std::runtime_error;
using std::string;

string SyntaxParser::getLineNumber()
{
	const size_t length = this->tokens->size();
	const int t = this->pos >= (int) length ? (int) length - 1 : this->pos;
	return std::format("(near line {})", (*this->tokens)[t]->lineNumber);
};

vector<Routine*>* SyntaxParser::parse()
{
	std::unique_ptr<vector<Routine*>> routines = std::make_unique<vector<Routine*>>();
	const vector<Token::token*>& tokens = *this->tokens;
	const size_t length = tokens.size();
	int& t = this->pos;

	try
	{
		while (t < length)
			routines->push_back(parseRoutine());
	}
	catch (std::exception error)
	{
		deleteAll(routines.get());
		throw error;
	}

	return routines.release();
}

Routine* SyntaxParser::parseRoutine()
{
	MetaParser parser(this);

	// Header
	parser.verifyKeyword(Token::Keyword::kw_routine);
	const string* name = parser.getString();

	parser.verifyKeyword(Token::Keyword::kw_using);

	std::unique_ptr<vector<string*>> parameters(parser.getStringList());
	std::unique_ptr<vector<Variable*>> variables = std::make_unique<vector<Variable*>>();

	for (string* param : *parameters)
		if (*param == "nothing" && parameters->size() > 1)
		{
			deleteAll(variables.get());
			throw runtime_error(format("cannot use \"nothing\" with other parameters (in routine {})", *name));
		}
		else
			variables->push_back(new Variable(*param));

	if (parameters->size() == 0)
	{
		deleteAll(variables.get());
		throw runtime_error(format("invalid number of parameters (in routine {})", *name));
	}

	/*std::cout << "Got parameters.\n";
	for (Variable*& var : *variables)
		std::cout << var->name << "\n";*/

	std::unique_ptr<Routine> routine(new Routine(
		*name,
		parameters->size() - (*parameters->at(0) == "nothing" ? 1 : 0),
		variables.release()
	));

	// Body
	std::unique_ptr<vector<std::pair<string*, ExpressionNode*>>> definitions(parser.parseVarDefList());

	for (std::pair<string*, ExpressionNode*>& result : *definitions)
	{
		routine->variables->push_back(new Variable(*result.first));
		if (result.second)
			routine->subtrees->push_back(result.second);
	}

	std::unique_ptr<vector<AbstractNode*>> block(parser.parseBlock());

	for (AbstractNode*& node : *block)
		routine->subtrees->push_back(node);

	return routine.release();
}