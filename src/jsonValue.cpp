#include <assert.h>
#include <string>
#include "jsonValue.h"
#include "jsonParser.h"

namespace json {

	Value& Value::operator=(const Value &rhs) noexcept
	{
		free();
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
				break;
			case json::String: str_ = rhs.str_;
				break;
			case json::Array:  arr_ = rhs.arr_;
				break;
		}
	}
	void Value::free() noexcept
	{
		using std::string;
		switch (type_) {
			case json::String: str_.~string();
				break;
			case json::Array: arr_.~vector<Value>();
				break;
		}
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
			free();
			type_ = json::String;
			new(&str_) std::string(str);
		}
	}
	size_t Value::get_array_size() const noexcept
	{
		assert(type_ == json::Array);
		return arr_.size();
	}
	const Value& Value::get_array_element(size_t index) const noexcept
	{
		assert(type_ == json::Array);
		return arr_[index];
	}
	void Value::set_array(const std::vector<Value> &arr) noexcept
	{
		if (type_ == json::Array)
			arr_ = arr;
		else {
			free();
			type_ = json::Array;
			new(&arr_) std::vector<Value>(arr);
		}
	}

	void Value::parse(const std::string &content)
	{
		Parser(*this, content);
	}
}
