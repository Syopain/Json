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

	void Value::parse(const std::string &content) noexcept
	{
		Parser(*this, content);
	}
}
