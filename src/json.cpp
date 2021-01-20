#include "json.h"
#include "jsonValue.h"
#include "jsonException.h"

namespace syo {

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
		v-> parse(content);
	}

	void Json::stringify(std::string &content) const noexcept
	{
		v-> stringify(content);
	}


	Json::Json() noexcept : v(new json::Value) { }

	Json::~Json() noexcept { }

	Json::Json(const Json &rhs) noexcept
	{
		v.reset(new json::Value(*(rhs.v)));
	}

	Json& Json::operator=(const Json &rhs) noexcept
	{
		v.reset(new json::Value(*(rhs.v)));
		return *this;
	}

	Json::Json(Json &&rhs) noexcept
	{
		v.reset(rhs.v.release());
	}

	Json& Json::operator=(Json &&rhs) noexcept
	{
		v.reset(rhs.v.release());
		return *this;
	}

	void Json::swap(Json &rhs) noexcept
	{
		using std::swap;
		swap(v, rhs.v);
	}

	void swap(Json &lhs, Json &rhs) noexcept
	{
		lhs.swap(rhs);
	}

	int Json::get_type() const noexcept
	{
		if (v == nullptr)
			return json::Null;
		return v-> get_type();
	}

	void Json::set_null() noexcept
	{
		v->set_type(json::Null);
	}

	void Json::set_boolean(bool b) noexcept
	{
		if(b) v->set_type(json::True);
		else v->set_type(json::False);
	}

	double Json::get_number() const noexcept
	{
		return v-> get_number();
	}

	void Json::set_number(double d) noexcept
	{
		v-> set_number(d);
	}

	const std::string Json::get_string() const noexcept
	{
		return v-> get_string();
	}

	void Json::set_string(const std::string &str) noexcept
	{
		v-> set_string(str);
	}

	size_t Json::get_array_size() const noexcept
	{
		return v-> get_array_size();
	}

	Json Json::get_array_element(size_t index) const noexcept
	{
		Json ret;
		ret.v.reset(new json::Value(v-> get_array_element(index)));
		return ret;
	}

	void Json::set_array() noexcept
	{
		v-> set_array(std::vector<json::Value>{});
	}

	void Json::pushback_array_element(const Json& val) noexcept
	{
		v-> pushback_array_element(*val.v);
	}

	void Json::popback_array_element() noexcept
	{
		v-> popback_array_element();
	}

	void Json::insert_array_element(const Json &val, size_t index) noexcept
	{
		v-> insert_array_element(*val.v, index);
	}

	void Json::erase_array_element(size_t index, size_t count) noexcept
	{
		v-> erase_array_element(index, count);
	}

	void Json::clear_array() noexcept
	{
		v-> clear_array();
	}

	void Json::set_object() noexcept
	{
		v-> set_object(std::vector<std::pair<std::string, json::Value>>{});
	}

	size_t Json::get_object_size() const noexcept
	{
		return v-> get_object_size();
	}

	const std::string& Json::get_object_key(size_t index) const noexcept
	{
		return v-> get_object_key(index);
    }

	size_t Json::get_object_key_length(size_t index) const noexcept
	{
		return v-> get_object_key_length(index);
    }

	Json Json::get_object_value(size_t index) const noexcept
	{
		Json ret;
		ret.v.reset(new json::Value(v-> get_object_value(index)));
		return ret;
    }

	void Json::set_object_value(const std::string &key, const Json &val) noexcept
	{
		v-> set_object_value(key, *val.v);
    }

	long long Json::find_object_index(const std::string &key) const noexcept
	{
		return v-> find_object_index(key);
    }

	void Json::remove_object_value(size_t index) noexcept
	{
		v-> remove_object_value(index);
    }

	void Json::clear_object() noexcept
	{
		v-> clear_object();
	}

	bool operator==(const Json &lhs, const Json &rhs) noexcept
	{
		return *lhs.v == *rhs.v;
    }

   	bool operator!=(const Json &lhs, const Json &rhs) noexcept
	{
		return *lhs.v != *rhs.v;
	}
}
