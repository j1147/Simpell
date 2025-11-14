#pragma once
#include <cassert>
#include <string>

class Value
{
public:
	enum class Which { Double, String } which;

	union Internal
	{
		double numeric_value;
		std::string* string_value;
	} value;

	Value(double value) : which(Value::Which::Double)
	{
		this->value = { .numeric_value = value };
	}

	Value(std::string* value) : which(Value::Which::String)
	{
		this->value = { .string_value = value };
	}

	void setString(std::string* value)
	{
		this->value.string_value = value;
		this->which = Value::Which::String;
	}

	void setDouble(double value)
	{
		if (which == Value::Which::String && this->value.string_value)
		{
			delete this->value.string_value;
			this->value.string_value = nullptr;
		}
		this->value.numeric_value = value;
		this->which = Value::Which::Double;
	}

	double getDouble() const
	{
		assert(this->which == Value::Which::Double);
		return this->value.numeric_value;
	}

	std::string* getString() const
	{
		assert(this->which == Value::Which::String);
		assert(this->value.string_value);
		return this->value.string_value;
	}

	Value* clone() const
	{
		if (this->which == Value::Which::String)
			return new Value(new std::string(*this->value.string_value));
		else if (this->which == Value::Which::Double)
			return new Value(this->value.numeric_value);

		assert("????");
		return nullptr;
	}

	~Value() {
		if (which == Value::Which::String && this->value.string_value)
		{
			delete this->value.string_value;
			this->value.string_value = nullptr;
		}
	}
};