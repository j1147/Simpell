#pragma once
#include "_global.hpp"
#include "BlockNode.hpp"

BlockNode::~BlockNode()
{
	for (AbstractNode*& subtree : *subtrees)
		delete subtree;
	delete subtrees;
	delete condition;
}