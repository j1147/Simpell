#pragma once
#include <string>

using std::string;

namespace Token
{
	constexpr char tab{ '\t' };
	constexpr char space{ ' ' };
	constexpr char newline{ '\n' };
	constexpr char newline2{ '\r' };
	bool isNewline(char character);
	bool isWhitespace(char character);

	constexpr char semicolon{ ';' };
	constexpr char colon{ ':' };
	constexpr char l_bracket{ '{' };
	constexpr char r_bracket{ '}' };
	constexpr char comma{ ',' };
	bool isControlFlow(char character);

	namespace Keyword
	{
		const string kw_true = "true";
		const string kw_false = "false";

		const string kw_routine = "routine";
		const string kw_using = "using";

		const string kw_define = "define";

		const string kw_during = "during";
		const string kw_if = "if";
		const string kw_else = "else";
		const string kw_done = "done";

		const string kw_yield = "yield";
		const string kw_call = "call";
		const string kw_with = "with";
	};

	namespace Operator
	{
		constexpr char assign{ '=' };
		constexpr char add{ '+' };
		constexpr char sub{ '-' };
		constexpr char mul{ '*' };
		constexpr char div{ '/' };
		constexpr char invert{ '!' };
		constexpr char mod{ '%' };
		constexpr char op_and{ '&' };
		constexpr char op_or{ '|' };
		constexpr char dot{ '.' };
		constexpr char l_parenthesis{ '(' };
		constexpr char r_parenthesis{ ')' };
		bool isOperator(char character);
	};


	constexpr char underscore{ '_' };
	bool isIdentifierValid(char character);
	bool isNumber(char character);

	enum class Type {
		STRING,
		NUMBER,
		CONTROL_FLOW,
		OPERATOR
	};

	struct token
	{
		Token::Type type;
		string content;
		token(Token::Type type, string content) : type(type), content(content) {};
		token(Token::Type type, char character) : type(type), content(string(1, character)) {};
	};

	string getTokenName(token* token);
};