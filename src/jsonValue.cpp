#include <assert.h>
#include <string>
#include "jsonValue.h"
#include "jsonParser.h"
#include "jsonGenerator.h"

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
			case json::String: new(&str_) std::string(rhs.str_);
				break;
			case json::Array:  new(&arr_) std::vector<Value>(rhs.arr_);
				break;
			case json::Object: new(&obj_) std::vector<std::pair<std::string, Value>>(rhs.obj_);
				break;
		}
	}
	void Value::free() noexcept
	{
		using std::string;
		switch (type_) {
			case json::String: str_.~string();
				break;
			case json::Array:  arr_.~vector<Value>();
				break;
			case json::Object: obj_.~vector<std::pair<std::string, Value>>();
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
	const std::string& Value::get_string() const noexcept
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
	size_t Value::get_object_size() const noexcept
	{
		assert(type_ == json::Object);
		return obj_.size();
	}
	const std::string& Value::get_object_key(size_t index) const noexcept
	{
		assert(type_ == json::Object);
		return obj_[index].first;
	}
	size_t Value::get_object_key_length(size_t index) const noexcept
	{
		assert(type_ == json::Object);
		return obj_[index].first.size();
	}
	const Value& Value::get_object_value(size_t index) const noexcept
	{
		assert(type_ == json::Object);
		return obj_[index].second;
	}
	void Value::set_object(const std::vector<std::pair<std::string, Value>> &obj) noexcept
	{
		if(type_ == json::Object)
			obj_ = obj;
		else{
			free();
			type_ = json::Object;
			new(&obj_) std::vector<std::pair<std::string, Value>>(obj);
		}
	}
	long long Value::find_object_index(const std::string &key) const noexcept
	{
		assert(type_ == json::Object);
		for(int i = 0; i < obj_.size(); ++i) {
			if(obj_[i].first == key)
				return i;
		}
		return -1;
	}

	void Value::parse(const std::string &content)
	{
		Parser(*this, content);
	}

	void Value::stringify(std::string &content) const noexcept
	{
		Generator(*this, content);
	}

	bool operator==(const Value &lhs, const Value &rhs) noexcept
	{
		if (lhs.get_type() != rhs.get_type())
			return false;
		switch (lhs.get_type()) {
			case json::String: return lhs.str_ == rhs.str_;
			case json::Number: return lhs.num_ == rhs.num_;
			case json::Array:  return lhs.arr_ == rhs.arr_;
			case json::Object:
				if (lhs.get_object_size() != rhs.get_object_size())
					return false;
				for (size_t i = 0; i < lhs.get_object_size(); ++i) {
					auto index = rhs.find_object_index(lhs.get_object_key(i));
					if(index < 0 || lhs.get_object_value(i) != rhs.get_object_value(index)) return false;
				}
				return true;
		}
		return true;
	}
	bool operator!=(const Value &lhs, const Value &rhs) noexcept
	{
		return !(lhs == rhs);
	}

}
