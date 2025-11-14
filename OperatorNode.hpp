#pragma once
#include "AbstractBranchNode.hpp"
#include "AbstractNode.hpp"
#include "Token.hpp"
#include <string>
#include <format>
#include <vector>
#include <cassert>

using std::string;
using std::vector;
using std::format;

class OperatorNode : public AbstractBranchNode
{
public:
	AbstractNode* left;
	AbstractNode* right;

	OperatorNode(const Token::token* token, unsigned int localPrecedence) :
		AbstractBranchNode(token, Token::Operator::isUnaryOperator(token->first()), localPrecedence),
		left(nullptr),
		right(nullptr)
	{};

	char opcode() const { return token->first(); }
	bool isFull() const;
	void disown(AbstractNode* node);
	void consume(AbstractNode* node);
	AbstractNode* getRightmostNode() const;
	void log() const;
	string name() const { return format("OperatorNode {}", opcode()); };

	void dumpToStack(vector<const AbstractNode*>* stack) const;

	~OperatorNode();
};