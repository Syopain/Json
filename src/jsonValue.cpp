#include "jsonValue.h"
#include "jsonParser.h"

namespace json {

	int Value::get_type() const noexcept
	{
		return type_;
	}
	int Value::set_type(type t) noexcept
	{
		return type_ = t;
	}
	double Value::get_number() const noexcept
	{
		return num_;
	}

	void Value::parse(const std::string &content)
	{
		Parser(*this, content);
	}
}
