#pragma once
#include "Token.hpp"
#include "ExpressionReader.hpp"
#include "ExpressionNode.hpp"
#include "AbstractNode.hpp"
#include "SyntaxParser.hpp"
#include <string>
#include <vector>
#include <format>

using std::string;
using std::vector;

/*struct Element
{
public:
	enum Type {
		STRING,
		KEYWORD,
		VAR_DEF_LIST, // List of variable definitions
		BLOCK, // Set of expressions, blocks, and statements
		EXPRESSION, // Non-statement expression
		STRING_LIST, // List of comma-separated strings, terminated by :
		EXPRESSION_LIST, // List of expressions, terminated by : or ;
		VAR_DEF // Expression of the form "var = expr" or "var"
	};

	Element::Type type;
	ExpressionReader::Context context;
};*/

class MetaParser
{
public:
	SyntaxParser* parser;

	MetaParser(SyntaxParser* parser) : parser(parser) {};

	void verifyKeyword(const Token::token& keyword);
	bool softVerifyKeyword(const Token::token& keyword);
	bool softVerifyTerminator(const ExpressionReader::Context context);
	string* getString();
	string* getStringOrNothing();
	vector<std::pair<string*, ExpressionNode*>>* parseVarDefList();
	vector<AbstractNode*>* parseBlock();
	ExpressionNode* parseExpression(ExpressionReader::Context context);
	vector<ExpressionNode*>* parseExpressionList(ExpressionReader::Context context);
	vector<string*>* getStringList();

	string format(string input)
	{
		return input + " " + this->parser->getLineNumber();
	}

	template <typename... Args>
	string format(string input, Args... values)
	{
		//return "an error occured";
		return std::vformat(input, std::make_format_args(values...)) + " " + this->parser->getLineNumber();
	}
};


/* parse({
Element{ abstractType: KEYWORD, content: kw_call },
Element{ STRING },
KeywordElement{ content: kw_with },
ExpressionListElement{ abstractType: EXPRESSION_LIST



ROUTINE_HEADER: routine {string} using {string_list}: {vardef_list} {block}

IF_BLOCK: if {expression}: {block} else: {block}

DURING_BLOCK: during {expression}: {block}

FUNCTION_CALL: call {string} with {expression_list};

YIELD_STATEMENT: yield {expression};

DONE_STATEMENT: done;

DEFINE_STATEMENT: define {var_expression};

{expression}: {string} = {expression} | {expression}
{var_expression}: {string} = {expression} | {string}



function_call would be like

MetaParser mp(syntaxParser);

mp.verifyKeyword(kw_call);
string name = mp.getString();
mp.checkKeyword(kw_with);
vector<Expression*>* args = mp.parseExpressionList(context = INLINE | CONDITION);


DEFINE_STATEMENT would be like

MetaParser mp(syntaxParser);
mp.verifyKeyword(kw_define);
std::pair<string, ExpressionNode> result = mp.parseVariableDefinition();


ROUTINE_HEADER would be like*/