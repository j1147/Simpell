#pragma once
#include "GenericNode.hpp"
#include "Token.hpp"

class OperatorNode : public GenericNode
{
public:
	GenericNode* left;
	GenericNode* right;
	OperatorNode* parent;
	
	bool isUnary;
	char opcode;
	unsigned int localPrecedence;

	OperatorNode(char opcode, unsigned int localPrecedence) :
		opcode(opcode),
		left(nullptr),
		right(nullptr),
		parent(nullptr),
		isUnary(Token::Operator::isUnaryOperator(opcode)),
		localPrecedence(localPrecedence)
	{};

	bool isFull() const;
	void disown(GenericNode* node);
	void consume(GenericNode* node);
	GenericNode* getRightmostNode() const;
	void log();
};