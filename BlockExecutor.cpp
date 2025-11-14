#pragma once
#include "_global.hpp"
#include "BlockExecutor.hpp"
#include "ExpressionExecutor.hpp"
#include "ExpressionNode.hpp"
#include "Value.hpp"
#include "RoutineContext.hpp"
#include "Token.hpp"
#include "YieldNode.hpp"

#include <iostream>

bool BlockExecutor::execute(RoutineContext* context) const
{
	assert(this->block);

	if (this->block->condition)
	{
		ExpressionExecutor executor(this->block->condition);
		Value* result = executor.execute(context, ExpressionReader::Context::CONDITION);

		if (!Executor::isTruthy(result))
			return false;

		delete result;
	}

	const size_t length = this->block->subtrees->size();

	bool branchToNextElse = false;

	for (int b = 0; b < length; ++b)
	{
		const AbstractNode* node = (*this->block->subtrees)[b];

		if (node->token && node->token->type == Token::Type::KEYWORD)
		{
			if (node->token->content == Token::Keyword::kw_if.content)
			{
				// if block
				BlockExecutor executor(dynamic_cast<const BlockNode*>(node));
				if (!executor.execute(context))
				{
					branchToNextElse = true;
					continue;
				}
			}
			else if (branchToNextElse && node->token->content == Token::Keyword::kw_else.content)
			{
				// else block
				BlockExecutor executor(dynamic_cast<const BlockNode*>(node));
				executor.execute(context);
			}
			else if (node->token->content == Token::Keyword::kw_during.content)
			{
				// during block
				BlockExecutor executor(dynamic_cast<const BlockNode*>(node));
				while (executor.execute(context));
			}
			else if (node->token->content == Token::Keyword::kw_yield.content)
			{
				// yield statement
				const YieldNode* yield = dynamic_cast<const YieldNode*>(node);
				assert(yield);

				ExpressionExecutor executor(yield->value);
				if (context->returnValue)
					delete context->returnValue;
				context->returnValue = executor.execute(context, ExpressionReader::Context::CONDITION);
			}
		}
		else if (const ExpressionNode* expression = dynamic_cast<const ExpressionNode*>(node))
		{
			// expression statement
			ExpressionExecutor executor(expression);
			Value* noresult = executor.execute(context, ExpressionReader::Context::INLINE);
			if (noresult)
				delete noresult;
		}

		branchToNextElse = false;
	}
	return true;
}