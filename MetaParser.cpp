#pragma once
#include "_global.hpp"
#include "MetaParser.hpp"
#include "Token.hpp"
#include "ExpressionReader.hpp"
#include "OperatorNode.hpp"
#include "YieldNode.hpp"
#include <string>
#include <stdexcept>
#include <vector>
#include <memory>

using std::string;
using std::runtime_error;
using std::vector;

void MetaParser::verifyKeyword(const Token::token& keyword)
{
	const vector<Token::token*>& tokens = *this->parser->tokens;
	const size_t length = tokens.size();
	int& t = this->parser->pos;

	if (t >= length)
		throw runtime_error(format("unexpected end of file"));

	if (tokens[t]->type != Token::Type::KEYWORD || tokens[t]->content != keyword.content)
		throw runtime_error(format("expected {} keyword, but got {} instead", keyword.content, tokens[t]->content));

	++t;
}

bool MetaParser::softVerifyKeyword(const Token::token& keyword)
{
	const vector<Token::token*>& tokens = *this->parser->tokens;
	const size_t length = tokens.size();
	int& t = this->parser->pos;

	if (t >= length)
		return false;

	if (!Token::equals(tokens[t], &keyword))
		return false;

	++t;
	return true;
}

bool MetaParser::softVerifyTerminator(const ExpressionReader::Context context)
{
	const vector<Token::token*>& tokens = *this->parser->tokens;
	const size_t length = tokens.size();
	int& t = this->parser->pos;

	if (t >= length)
		return false;
	if (tokens[t]->type != Token::Type::CONTROL_FLOW)
		return false;

	if (context == ExpressionReader::Context::CONDITION && tokens[t]->first() != Token::colon)
		return false;
	else if (context == ExpressionReader::Context::INLINE && tokens[t]->first() != Token::semicolon)
		return false;
	
	++t;
	return true;
}

string* MetaParser::getString()
{
	const vector<Token::token*>& tokens = *this->parser->tokens;
	const size_t length = tokens.size();
	int& t = this->parser->pos;

	if (t >= length)
		throw runtime_error(format("unexpected end of file"));

	if (tokens[t]->type != Token::Type::NAME)
		throw runtime_error(format("expected a name or string, but got {} instead", tokens[t]->content));

	// !!!
	return &tokens[t++]->content;
}

string* MetaParser::getStringOrNothing()
{
	const vector<Token::token*>& tokens = *this->parser->tokens;
	const size_t length = tokens.size();
	int& t = this->parser->pos;

	if (t >= length)
		throw runtime_error(format("unexpected end of file"));

	if (tokens[t]->type != Token::Type::NAME
		&& (tokens[t]->type != Token::Type::KEYWORD || tokens[t]->content != Token::Keyword::kw_nothing.content))
		throw runtime_error(format("expected a name or string, but got {} instead", tokens[t]->content));

	// !!!
	return &tokens[t++]->content;
}

vector<std::pair<string*, ExpressionNode*>>* MetaParser::parseVarDefList()
{
	const vector<Token::token*>& tokens = *this->parser->tokens;
	const size_t length = tokens.size();
	int& t = this->parser->pos;

	std::unique_ptr<vector<std::pair<string*, ExpressionNode*>>> out = std::make_unique<vector<std::pair<string*, ExpressionNode*>>>();

	try
	{
		while (softVerifyKeyword(Token::Keyword::kw_define))
		{
			string* name = getString();
			--t;

			ExpressionNode* initialization = parseExpression(ExpressionReader::Context::INLINE);
			if (initialization && initialization->top)
			{
				if (initialization->top->token->type == Token::Type::OPERATOR && initialization->top->token->first() == Token::Operator::equal.first())
					// Assignment
					out->push_back(std::make_pair(
						name,//((OperatorNode*) initialization->top)->left->token->content,
						initialization
					));
				else if (initialization->top->token->type == Token::Type::NAME)
					// Variable
					out->push_back(std::make_pair(
						name,//initialization->top->token->content,
						nullptr
					));
				else
					throw runtime_error(format("expected a variable definition"));
			}
			else throw runtime_error(format("invalid exression"));
		}
	}
	catch (std::exception error)
	{
		for (std::pair<string*, ExpressionNode*>& pair : *out)
		{
			delete pair.second;
			pair.second = nullptr;
			pair.first = nullptr;
		}
		out->clear();
		throw error;
	}

	return out.release();
}

ExpressionNode* MetaParser::parseExpression(ExpressionReader::Context context)
{
	ExpressionReader reader(this->parser);
	return reader.parseExpression(context);
}

vector<ExpressionNode*>* MetaParser::parseExpressionList(ExpressionReader::Context context)
{
	const vector<Token::token*>& tokens = *this->parser->tokens;
	const size_t length = tokens.size();
	Token::token* token = nullptr;
	int& t = this->parser->pos;
	std::unique_ptr<vector<ExpressionNode*>> nodes = std::make_unique<vector<ExpressionNode*>>();

	try
	{
		while (t < length)
		{
			// Force breaks on comma
			ExpressionNode* expression = parseExpression(ExpressionReader::Context::FUNCTION_ARGUMENT);
			if (!expression || t <= 0)
				throw runtime_error(format("invalid expression"));
			token = tokens[t - (size_t)1];

			nodes->push_back(expression);

			if (token->type == Token::Type::CONTROL_FLOW)
			{
				if (token->first() == Token::semicolon)
					if (context != ExpressionReader::Context::INLINE)
						throw runtime_error(format("unexpected token {}, expected a :", token->content));
					else break;
				else if (token->first() == Token::colon)
					if (context != ExpressionReader::Context::CONDITION)
						throw runtime_error(format("unexpected token {}, expected a ;", token->content));
					else break;
				else if (token->first() != Token::comma)
					break;
			}
			else throw runtime_error(format("internal error: malformed expression sequence"));
		}
	}
	catch (std::exception error)
	{
		deleteAll(nodes.get());
		throw error;
	}
	// Must end on a ; or :, but this is checked inside ExpressionReader::parseExpression

	return nodes.release();
}

vector<AbstractNode*>* MetaParser::parseBlock()
{
	const vector<Token::token*>& tokens = *this->parser->tokens;
	const size_t length = tokens.size();
	int& t = this->parser->pos;
	std::unique_ptr<vector<AbstractNode*>> nodes = std::make_unique<vector<AbstractNode*>>();

	try
	{
		while (t < length && !softVerifyKeyword(Token::Keyword::kw_done))
		{
			// C++ does NOT respect order of arguments, something that was taken
			// for granted in this section.
			const Token::token* keyword = tokens[t];
			if (softVerifyKeyword(Token::Keyword::kw_yield))
				nodes->push_back(new YieldNode(
					keyword,
					parseExpression(ExpressionReader::Context::INLINE)
				));
			else if (softVerifyKeyword(Token::Keyword::kw_during))
			{
				ExpressionNode* expression = parseExpression(ExpressionReader::Context::CONDITION);
				nodes->push_back(new BlockNode(
					keyword,
					expression,
					parseBlock()
				));
			}
			else if (softVerifyKeyword(Token::Keyword::kw_if))
			{
				ExpressionNode* expression = parseExpression(ExpressionReader::Context::CONDITION);
				nodes->push_back(new BlockNode(
					keyword,
					expression,
					parseBlock()
				));
			}
			else if (softVerifyKeyword(Token::Keyword::kw_else))
			{
				if (nodes->size() == 0 || (*nodes)[nodes->size() - 1]->token->content != Token::Keyword::kw_if.content)
					throw runtime_error(format("dangling else block"));

				if (t >= length)
					throw runtime_error(format("unexpected end of file"));

				if (tokens[t]->type != Token::Type::CONTROL_FLOW || tokens[t]->first() != Token::colon)
					throw runtime_error(format("missing : after else"));
				++t;


				nodes->push_back(new BlockNode(
					keyword,
					parseBlock())
				);
			}
			else if (tokens[t]->type == Token::Type::KEYWORD && tokens[t]->content != Token::Keyword::kw_call.content)
				throw runtime_error(format("unexpected token {}, delete this", tokens[t]->content));
			else
				nodes->push_back(parseExpression(ExpressionReader::Context::INLINE));
		}

		if (t >= length || tokens[t]->first() != Token::semicolon)
			throw runtime_error(format("expected ;"));
	}
	catch (std::exception error)
	{
		// Avoid having an array of std::unique_ptr
		deleteAll(nodes.get());
		throw error;
	}

	++t;

	return nodes.release();
}

vector<string*>* MetaParser::getStringList()
{
	const vector<Token::token*>& tokens = *this->parser->tokens;
	const size_t length = tokens.size();
	int& t = this->parser->pos;
	std::unique_ptr<vector<string*>> strings = std::make_unique<vector<string*>>();

	while (t < length)
	{
		strings->push_back(getStringOrNothing());

		if (t >= length)
			throw runtime_error(format("unexpected end of file"));

		if (tokens[t]->type == Token::Type::CONTROL_FLOW)
			if (tokens[t]->first() == Token::colon)
				break;
			else if (tokens[t]->first() == Token::comma)
			{
				++t;
				continue;
			}
		throw runtime_error(format("unexpected token {}, delete this", tokens[t]->content));
	}

	if (t >= length)
		throw runtime_error(format("unexpected end of file"));

	++t;
	
	return strings.release();
}