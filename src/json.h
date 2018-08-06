#ifndef JSON_H
#define JSON_H

#include <memory>
#include <string>

namespace json {

	enum type : int{
		Null,
		True,
		False,
		Number,
		String,
		Array,
		Object
	};

	class Value;

}

namespace pain{

	class Json{
	public:
		void parse(const std::string &content, std::string &status) noexcept;
		void parse(const std::string &content);

		Json();
		~Json();
		Json(const Json &src);

		int get_type() const noexcept;
		int set_type(json::type t) noexcept;
	private:
		std::unique_ptr<json::Value> v;
	};

}
#endif
