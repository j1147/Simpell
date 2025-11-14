#pragma once
#include "_global.hpp"
#include "ExpressionNode.hpp"

ExpressionNode::~ExpressionNode()
{
	delete this->top;
}