#pragma once
#include <iostream>

class GenericNode
{
public:
	GenericNode* parent;

	GenericNode() : parent(nullptr)
	{
		std::cout << "construction of genericnode\n";
	};

	virtual void disown(GenericNode* child) {};
	virtual void consume(GenericNode* node) {};
	virtual void log() {};

	~GenericNode()
	{
		std::cout << "destruction of genericnode\n";
	}
};