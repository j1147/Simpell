#pragma once
#include "AbstractNode.hpp"
#include <vector>
#include <string>
#include <format>
#include <iostream>

using std::string;
using std::vector;
using std::format;

class NumberNode : public AbstractNode
{
public:
	double value;

	NumberNode(const Token::token* token) : AbstractNode(token), value(std::stod(token->content)) {};

	void log() const { std::cout << "Number " << token->content; };
	string name() const { return format("NumberNode {}", this->value); };
};