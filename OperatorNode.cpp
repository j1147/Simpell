#pragma once
#include "_global.hpp"
#include "AbstractNode.hpp"
#include "OperatorNode.hpp"
#include <stdexcept>
#include <format>
#include <iostream>
#include <vector>

using std::runtime_error;
using std::format;
using std::vector;

bool OperatorNode::isFull() const
{
	return isUnary ?
		left != nullptr
		:
		left != nullptr && right != nullptr;
}

void OperatorNode::disown(AbstractNode* node)
{
	if (left == node)
		left = right;
	else if (right == node)
		right = nullptr;
	else
		return;

	node->parent = nullptr;
}

void OperatorNode::consume(AbstractNode* node)
{
	if (left == nullptr)
		left = node;
	else if (!isUnary && right == nullptr)
		right = node;
	else
		throw runtime_error(format("internal error: cannot add child node to full {} node", isUnary ? "unary" : "binary"));

	if (node->parent)
	{
		AbstractBranchNode* parent = node->parent;
		parent->disown(node);
		parent->consume(this);
	}

	node->parent = this;
}

AbstractNode* OperatorNode::getRightmostNode() const
{
	return right ? right : left ? left : nullptr;
}

void OperatorNode::log() const
{
	std::cout << "Operator " << opcode() << " { ";
	if (left)
	{
		left->log();
		std::cout << " , ";
	}
	if (right)
		right->log();
	std::cout << " }";
}

void OperatorNode::dumpToStack(vector<const AbstractNode*>* stack) const {
	assert(this->isFull());
	left->dumpToStack(stack);
	if (right)
		right->dumpToStack(stack);

	stack->push_back(this);
}

OperatorNode::~OperatorNode()
{
	if (left)
	{
		delete left;
		left = nullptr;
	}
	if (right)
	{
		delete right;
		right = nullptr;
	}
	if (parent)
	{
		parent->disown(this);
		parent = nullptr;
	}
}