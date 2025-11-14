#pragma once
#include "AbstractNode.hpp"
#include "Token.hpp"
#include <string>
#include <format>
#include <iostream>

using std::string;
using std::format;

class StringNode : public AbstractNode
{
public:
	StringNode(const Token::token* token) : AbstractNode(token) {};

	const string& value() const { return token->content; }
	void log() const { std::cout << "String " << value(); };
	string name() const { return format("StringNode {}", this->token->content); };
};