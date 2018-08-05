#ifndef JSON_H
#define JSON_H

#include <memory>
#include <string>

namespace json{

	enum type : int{
		NULL,
		TRUE,
		FALSE,
		NUMBER,
		STRING,
		ARRAY,
		OBJECT
	};

}

namespace pain{

	class json::value;

	class Json{
	public:
		Json parse(const std::string &content) noexcept;

		Json() = default;
		Json(const Json &src);
		Json& operator=(const Json &rhs);

		int type() const noexcept;
	private:
		unique_ptr<json::value> v;
	};

}
#endif
