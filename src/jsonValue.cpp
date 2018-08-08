#include <assert.h>
#include <string>
#include "jsonValue.h"
#include "jsonParser.h"

namespace json {

	Value& Value::operator=(const Value &rhs) noexcept
	{
		using std::string;
		if (type_ == json::String)
			str_.~string();
		init(rhs);
	}
	
	Value::~Value() noexcept
	{
		using std::string;
		if(type_ == json::String)
			str_.~string();
	}

	void Value::init(const Value &rhs) noexcept
	{
		type_ = rhs.type_;
		num_ = 0;
		switch (type_) {
			case json::Number: num_ = rhs.num_;
			case json::String: str_ = rhs.str_;
		}
	}

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
		type_ = json::Number;
		num_ = d;
	}
	const std::string Value::get_string() const noexcept
	{
		assert(type_ == json::String);
		return "";
	}
	void Value::set_string(const std::string& str) noexcept
	{
		type_ = json::String;
		str_ = str;
	}

	void Value::parse(const std::string &content)
	{
		Parser(*this, content);
	}
}
