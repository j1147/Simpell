#pragma once
#include "_global.hpp"
#include "RoutineCallNode.hpp"

RoutineCallNode::~RoutineCallNode()
{
	for (ExpressionNode*& arg : *args)
		delete arg;
	delete args;
}