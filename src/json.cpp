#include "json.h"
#include "jsonValue.h"
#include "jsonException.h"

namespace pain {

	void Json::parse(const std::string &content, std::string &status) noexcept 
	{
		try{
			parse(content);
			status = "parse ok";
		} catch (const json::Exception &msg) {
			status = msg.what();
		} catch (...) {
		}
	}

	void Json::parse(const std::string &content)
	{
		v->parse(content);
	}

	Json::Json() : v(new json::Value) { }
	Json::~Json() {}
	Json::Json(const Json &src)
	{
		v.reset(new json::Value(*(src.v)));
	}

	int Json::get_type() const noexcept
	{
		return v-> get_type();
	}
	void Json::set_type(json::type t) noexcept
	{
		v-> set_type(t);
	}
	double Json::get_number() const noexcept
	{
		return v-> get_number();
	}
	void Json::set_number(double d) noexcept
	{
		v-> set_number(d);
	}

}
