#pragma once
#include "AbstractBranchNode.hpp"
#include "ExpressionNode.hpp"
#include <string>


class YieldNode : public AbstractBranchNode
{
public:
	ExpressionNode* value;

	YieldNode(const Token::token* statement, ExpressionNode* value) : AbstractBranchNode(statement), value(value) {};

	AbstractNode* getRightmostNode() const { return nullptr; };
	void disown(AbstractNode* node) {};
	void consume(AbstractNode* node) {};
	bool isFull() const { return true; };

	void log() const {};
	std::string name() const { return "YieldNode"; }
	~YieldNode() {
		delete value;
	};
};