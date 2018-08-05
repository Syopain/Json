#include "json.h"
#include "jsonValue.h"

namespace pain {

	void Json::parse(const std::string &content) noexcept
	{
		v->parse(content);
	}

	Json::Json() : v(new json::Value) { }

	Json::Json(const Json &src)
	{
		v.reset(new json::Value(*(src.v)));
	}

	int Json::get_type() const noexcept
	{
		return v-> get_type();
	}
	int Json::set_type(json::type t) noexcept
	{
		return v-> set_type(t);
	}

}
