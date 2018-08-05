#ifndef JSON_H
#define JSON_H

#include <memory>
#include <string>

namespace json {

	enum type : int{
		JSON_NULL,
		JSON_TRUE,
		JSON_FALSE,
		JSON_NUMBER,
		JSON_STRING,
		JSON_ARRAY,
		JSON_OBJECT
	};

	class Value;

}

namespace pain{

	class Json{
	public:
		Json parse(const std::string &content) noexcept;

		Json() = default;
//		Json(const Json &src);

		int get_type() const noexcept;
		int set_type(json::type t) noexcept;
	private:
		std::shared_ptr<json::Value> v;
//		json::Value *v;
	};

}
#endif
