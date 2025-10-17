#pragma once
#include "CodeWrapper.hpp"

class BlockReader
{
public:
	CodeWrapper* wrapper;

	BlockReader(CodeWrapper* wrapper) : wrapper(wrapper) {};
	bool read();
};