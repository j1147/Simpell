#pragma once

// Wrapper for code information
class CodeWrapper
{
public:
	const char* code;
	const size_t length;
	unsigned int pos;
	unsigned int lineNumber;

	CodeWrapper(const char* code, const size_t length, int pos) :
		code(code),
		length(length),
		pos(pos),
		lineNumber(1)
	{};

	void skipWhitespaces();

	~CodeWrapper();
};