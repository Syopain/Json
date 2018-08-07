#include <assert.h>
#include "jsonValue.h"
#include "jsonParser.h"

namespace json {

	int Value::get_type() const noexcept
	{
		return type_;
	}
	void Value::set_type(type t) noexcept
	{
		type_ = t;
	}
	double Value::get_number() const noexcept
	{
		assert(type_ == json::Number);
		return num_;
	}
	void Value::set_number(double d) noexcept
	{
		num_ = d;
	}

	void Value::parse(const std::string &content)
	{
		Parser(*this, content);
	}
}
