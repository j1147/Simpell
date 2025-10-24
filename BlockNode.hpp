#pragma once
#include "GenericNode.hpp"
#include <vector>

using std::vector;

class BlockNode : public GenericNode
{
public:
	vector<GenericNode*>* subtrees;
};