#pragma once

// Wrapper for code information
class CodeWrapper
{
public:
	char* code;
	long length;
	int pos;

	CodeWrapper(char* code, long length, int pos) : code(code), length(length), pos(pos) {};

	void skipWhitespaces();

	~CodeWrapper();
};