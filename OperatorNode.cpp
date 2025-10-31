#pragma once
#pragma pointers_to_members( full_generality, virtual_inheritance )
#include "OperatorNode.hpp"
#include <stdexcept>
#include <format>
#include <iostream>

using std::runtime_error;
using std::format;

bool OperatorNode::isFull() const
{
	return isUnary ?
		left != nullptr
		:
		left != nullptr && right != nullptr;
}

void OperatorNode::disown(GenericNode* node)
{
	if (left == node)
		left = right;
	else if (right == node)
		right = nullptr;
	else
		return;

	node->parent = nullptr;
}

void OperatorNode::consume(GenericNode* node)
{
	if (left == nullptr)
		left = node;
	else if (!isUnary && right == nullptr)
		right = node;
	else
		throw runtime_error(format("internal error: cannot add child node to full {} node", isUnary ? "unary" : "binary"));

	if (node->parent)
	{
		GenericNode* parent = node->parent;

		parent->disown(node);
		parent->consume(this);
	}

	node->parent = this;
}

GenericNode* OperatorNode::getRightmostNode() const
{
	return right ? right : left ? left : nullptr;
}

void OperatorNode::log()
{
	std::cout << "Operator " << opcode << " { ";
	if (left)
	{
		left->log();
		std::cout << " , ";
	}
	if (right)
		right->log();
	std::cout << " }";
}