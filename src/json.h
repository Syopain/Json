#ifndef JSON_H
#define JSON_H

#include <memory>
#include <string>

namespace pain{

	class JsonValue;

	enum json_type : int{
		JSON_NULL,
		JSON_TRUE,
		JSON_FALSE,
		JSON_NUMBER,
		JSON_STRING,
		JSON_ARRAY,
		JSON_OBJECT
	};

	class Json{
	public:
		static Json parse(const std::string &content) noexcept;
		int type() const noexcept;
	private:
		unique_ptr<JsonValue> value;
	};

}
#endif
