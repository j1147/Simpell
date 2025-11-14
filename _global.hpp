#pragma once
#include <vector>
//#define DEBUG_OUTPUT

template <typename T>
void deleteAll(std::vector<T*>* vector)
{
	if (!vector)
		return;
	for (T* t : *vector)
		delete t;
	vector->clear();
}