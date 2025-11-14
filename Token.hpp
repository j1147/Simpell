#pragma once
#include <string>

using std::string;

namespace Token
{
	enum class Type {
		KEYWORD,
		NAME,
		NUMBER,
		CONTROL_FLOW,
		OPERATOR,
		STRING_LITERAL
	};

	struct token
	{
		Token::Type type;
		string content;
		unsigned int lineNumber;
		token(Token::Type type, string content) :
			type(type), content(content), lineNumber(0)
		{};
		token(Token::Type type, char character) :
			type(type), content(string(1, character)), lineNumber(0)
		{};
		token(Token::Type type, string content, unsigned int lineNumber) :
			type(type), content(content), lineNumber(lineNumber)
		{};
		token(Token::Type type, char character, unsigned int lineNumber) :
			type(type), content(string(1, character)), lineNumber(lineNumber)
		{};
		char first() const { return content[0]; };
	};

	bool equals(Token::token* a, Token::token* b);
	bool equals(Token::token* a, const Token::token* b);

	constexpr char tab{ '\t' };
	constexpr char space{ ' ' };
	constexpr char newline{ '\n' };
	constexpr char newline2{ '\r' };
	bool isNewline(char character);
	bool isWhitespace(char character);

	constexpr char semicolon{ ';' };
	constexpr char colon{ ':' };
	constexpr char comma{ ',' };
	bool isControlFlow(char character);

	constexpr char quote{ '"' };

	namespace Keyword
	{
		const token kw_true(Token::Type::KEYWORD, "true");
		const token kw_false(Token::Type::KEYWORD, "false");

		const token kw_routine(Token::Type::KEYWORD, "routine");
		const token kw_using(Token::Type::KEYWORD, "using");
		const token kw_nothing(Token::Type::KEYWORD, "nothing");

		const token kw_define(Token::Type::KEYWORD, "define");

		const token kw_during(Token::Type::KEYWORD, "during");
		const token kw_if(Token::Type::KEYWORD, "if");
		const token kw_else(Token::Type::KEYWORD, "else");
		const token kw_done(Token::Type::KEYWORD, "done");

		const token kw_yield(Token::Type::KEYWORD, "yield");
		const token kw_call(Token::Type::KEYWORD, "call");
		const token kw_with(Token::Type::KEYWORD, "with");

		bool isKeyword(string str);
	};

	namespace Operator
	{
		const token equal(Token::Type::OPERATOR, '=');
		const token less_than(Token::Type::OPERATOR, '<');
		const token greater_than(Token::Type::OPERATOR, '>');
		const token add(Token::Type::OPERATOR, '+');
		const token sub(Token::Type::OPERATOR, '-');
		const token mul(Token::Type::OPERATOR, '*');
		const token div(Token::Type::OPERATOR, '/');
		const token mod(Token::Type::OPERATOR, '%');
		const token invert(Token::Type::OPERATOR, '!');
		const token op_and(Token::Type::OPERATOR, '&');
		const token op_or(Token::Type::OPERATOR, '|');

		const token dot(Token::Type::OPERATOR, '.');
		const token l_parenthesis(Token::Type::OPERATOR, '(');
		const token r_parenthesis(Token::Type::OPERATOR, ')');

		const token OPERATOR_LIST[] = {
			equal,
			less_than,
			greater_than,
			add,
			sub,
			mul,
			div,
			mod,
			invert,
			op_and,
			op_or,
			//dot,
			l_parenthesis,
			r_parenthesis
		};

		constexpr unsigned int HIGHEST_PRECEDENCE{ 6u };
		bool isOperator(char character);
		bool isUnaryOperator(char character);
		const token* getOperator(char character);
		unsigned int getPrecedence(char character);
	};


	constexpr char underscore{ '_' };
	bool isIdentifierValid(char character);
	bool isNumber(char character);

	string getTokenName(token* token);
};