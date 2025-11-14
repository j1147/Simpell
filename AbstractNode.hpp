#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "Token.hpp"

class AbstractBranchNode;

using std::string;
using std::vector;

class AbstractNode
{
public:
	AbstractBranchNode* parent;
	const Token::token* token;

	AbstractNode() : parent(nullptr), token(nullptr) {};
	AbstractNode(const Token::token* token) : parent(nullptr), token(token) {};

	virtual void log() const = 0;
	virtual string name() const = 0;

	virtual void dumpToStack(vector<const AbstractNode*>* stack) const { stack->push_back(this); }
};