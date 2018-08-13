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

		Json() noexcept;
		~Json() noexcept;
		Json(const Json &rhs) noexcept;
		Json& operator=(const Json &rhs) noexcept;

		int get_type() const noexcept;
		void set_type(json::type t) noexcept;
		double get_number() const noexcept;
		void set_number(double d) noexcept;
		const std::string get_string() const noexcept;
		void set_string(const std::string& str) noexcept;
		size_t get_array_size() const noexcept;
		const json::Value& get_array_element(size_t index) const noexcept;
	private:
		std::unique_ptr<json::Value> v;
	};

}
#endif	//	JSON_H
