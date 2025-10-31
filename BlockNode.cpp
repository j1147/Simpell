#pragma once
#include "BlockNode.hpp"

BlockNode::~BlockNode()
{
	for (GenericNode*& subtree : *subtrees)
		delete subtree;
	delete subtrees;
}