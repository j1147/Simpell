#pragma once
#include "ExpressionNode.hpp"
#include "AbstractNode.hpp"
#include "Token.hpp"
#include <vector>
#include <format>

using std::vector;
using std::format;

class RoutineCallNode : public AbstractNode
{
public:
	string* routineName;
	vector<ExpressionNode*>* args;

	RoutineCallNode(string* routineName, vector<ExpressionNode*>* args) :
		AbstractNode(&Token::Keyword::kw_call),
		routineName(routineName),
		args(args)
	{};

	void log() const {};
	std::string name() const { return format("RoutineCallNode {}", *routineName); };
	~RoutineCallNode();
};