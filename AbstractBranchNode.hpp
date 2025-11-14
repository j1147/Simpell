#pragma once
#include "Token.hpp"
#include "AbstractNode.hpp"

class AbstractBranchNode : public AbstractNode
{
public:
	bool isUnary;
	unsigned int localPrecedence;

	//AbstractBranchNode() : isUnary(false), localPrecedence(0) {};
	AbstractBranchNode(const Token::token* token) :
		AbstractNode(token),
		isUnary(false),
		localPrecedence(0)
	{};
	AbstractBranchNode(const Token::token* token, bool isUnary, unsigned int localPrecedence) :
		AbstractNode(token),
		isUnary(isUnary),
		localPrecedence(localPrecedence)
	{};

	virtual AbstractNode* getRightmostNode() const = 0;
	virtual void disown(AbstractNode* node) = 0;
	virtual void consume(AbstractNode* node) = 0;
	virtual bool isFull() const = 0;
};