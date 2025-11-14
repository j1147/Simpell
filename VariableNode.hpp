#pragma once
#include "AbstractNode.hpp"
#include "Token.hpp"
#include <string>
#include <format>
#include <iostream>

using std::string;
using std::format;

class VariableNode : public AbstractNode
{
public:
	VariableNode(const Token::token* token) : AbstractNode(token) {};

	const string& variableName() const { return token->content; }
	void log() const { std::cout << "Variable " << variableName(); };
	string name() const { return format("VariableNode {}", this->token->content); };
};