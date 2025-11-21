#pragma once
#include "BlockReader.hpp"
#include <iostream>
#include "CodeWrapper.hpp"

bool BlockReader::read()
{
	std::cout << "Beginning to read...\n";
	char* code = this->wrapper->code;
	int i = this->wrapper->pos;

	int limit = sizeof(code);

	std::cout << "Size of the code: " << limit << '\n';

	std::cout << code[0] << code[1] << code[2] << '\n';

	return false;
}