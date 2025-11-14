#pragma once
#include <vector>
#include "Token.hpp"
#include "Routine.hpp"
#include <string>
#include <format>

using std::vector;
using std::string;

class SyntaxParser
{
public:
	const vector<Token::token*>* tokens;
	int pos;

	SyntaxParser(const vector<Token::token*>* tokens) : tokens(tokens), pos(0) {};
	string getLineNumber();


	vector<Routine*>* parse();
	Routine* parseRoutine();

	string format(string input)
	{
		return input + " " + getLineNumber();
	}

	template <typename... Args>
	string format(string input, Args... values)
	{
		//return "an error occured";
		return std::vformat(input, std::make_format_args(values...)) + " " + getLineNumber();
	}
};