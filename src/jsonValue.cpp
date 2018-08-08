#include <assert.h>
#include <string>
#include "jsonValue.h"
#include "jsonParser.h"

namespace json {

	Value& Value::operator=(const Value &rhs) noexcept
	{
		init(rhs);
	}
	
	Value::~Value() noexcept
	{
		free();
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
	void Value::free() noexcept
	{
		using std::string;
		if (type_ == json::String)
			str_.~string();
	}

	int Value::get_type() const noexcept
	{
		return type_;
	}
	void Value::set_type(type t) noexcept
	{
		free();
		type_ = t;
	}
	double Value::get_number() const noexcept
	{
		assert(type_ == json::Number);
		return num_;
	}
	void Value::set_number(double d) noexcept
	{
		free();
		type_ = json::Number;
		num_ = d;
	}
	const std::string Value::get_string() const noexcept
	{
		assert(type_ == json::String);
		return str_;
	}
	void Value::set_string(const std::string& str) noexcept
	{
		if (type_ == json::String)
			str_ = str;
		else {
			type_ = json::String;
			new(&str_) std::string(str);
		}
	}

	void Value::parse(const std::string &content)
	{
		Parser(*this, content);
	}
}
