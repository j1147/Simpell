#pragma once
#include "BlockNode.hpp"
#include "RoutineContext.hpp"

class BlockExecutor
{
public:
	const BlockNode* block;

	BlockExecutor(const BlockNode* block) : block(block) {};

	// Returns true if the block was entered and executed.
	bool execute(RoutineContext* context) const;
};