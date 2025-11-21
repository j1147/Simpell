#pragma once
#include <cassert>
#include <string>
#include <iostream>

class Value
{
public:
	enum class Which { Double, String } which;
	union Internal
	{
		double numeric_value;
		std::string* string_value;
	} value;
	bool deleted;

	Value(double value) : which(Value::Which::Double), deleted(false)
	{
		this->value = { .numeric_value = value };
	}

	Value(std::string* value) : which(Value::Which::String), deleted(false)
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
		{
			std::string* result = new std::string();
			*result = *this->value.string_value;
			return new Value(result);
		}
		else if (this->which == Value::Which::Double)
			return new Value(this->value.numeric_value);

		assert("????");
		return nullptr;
	}

	~Value() {
		deleted = true;
		if (which == Value::Which::String && this->value.string_value)
		{
			delete this->value.string_value;
			this->value.string_value = nullptr;
		}
	}
};