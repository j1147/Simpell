#pragma once
#include "SyntaxParser.hpp"
#include "Routine.hpp"
#include <vector>
#include <stdexcept>
#include <format>

#include <iostream>

using std::vector;
using Token::token;

vector<Routine*>* SyntaxParser::parse()
{
	vector<Routine*>* routines = new vector<Routine*>();
	vector<token*>::iterator it = tokens->begin();
	token* token;
	std::cout << "hione\n";
	while (it != tokens->end())
	{
		token = *it;
		std::cout << "hi\n";
		if (token->type != Token::Type::STRING)
		{
			throw std::runtime_error(std::format("unexpected token {}, delete this", token->content));
			break;
		}
		std::cout << "hi2\n";
		std::cout << token->content << "\n";
		std::cout << "hi2.5\n";
		if (token->content == Token::Keyword::kw_routine)
			routines->push_back(NULL);
		else
		{
			throw std::runtime_error(std::format("expected a routine definition, got {} instead", token->content));
			break;
		}
		std::cout << "hi33\n";
		++it;
	}

	return routines;
}