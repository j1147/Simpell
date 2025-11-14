#pragma once
#include "AbstractNode.hpp"
#include "ExpressionNode.hpp"
#include "Token.hpp"
#include <string>
#include <vector>

using std::vector;

class BlockNode : public AbstractNode
{
public:
	ExpressionNode* condition;
	vector<AbstractNode*>* subtrees;

	BlockNode(const Token::token* header) :
		AbstractNode(header),
		subtrees(new vector<AbstractNode*>()),
		condition(nullptr)
	{};
	BlockNode(const Token::token* header, vector<AbstractNode*>* subtrees) :
		AbstractNode(header),
		subtrees(subtrees),
		condition(nullptr)
	{};
	BlockNode(const Token::token* header, ExpressionNode* condition, vector<AbstractNode*>* subtrees) :
		AbstractNode(header),
		subtrees(subtrees),
		condition(condition)
	{};

	void log() const {};
	std::string name() const { return "BlockNode"; };
	~BlockNode();
};