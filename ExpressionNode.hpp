#pragma once
#include "GenericNode.hpp"

class ExpressionNode : public GenericNode
{
public:
	GenericNode* top;

	ExpressionNode(GenericNode* top) : top(top) {};
};