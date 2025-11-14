#pragma once
#include "AbstractNode.hpp"

class ExpressionNode : public AbstractNode
{
public:
	AbstractNode* top;

	ExpressionNode(AbstractNode* top) : top(top) {};

	void log() const {};
	std::string name() const { return "ExpressionNode"; };

	~ExpressionNode();
};