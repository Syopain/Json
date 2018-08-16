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
		void stringify(std::string &content) const noexcept;

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
		const Json get_array_element(size_t index) const noexcept;
		size_t get_object_size() const noexcept;
		const std::string& get_object_key(size_t index) const noexcept;
		size_t get_object_key_length(size_t index) const noexcept;
		Json get_object_value(size_t index) const noexcept;
		long long find_object_index(const std::string &key) const noexcept;
	private:
		std::unique_ptr<json::Value> v;
		friend bool operator==(const Json &lhs, const Json &rhs) noexcept;
		friend bool operator!=(const Json &lhs, const Json &rhs) noexcept;
	};
	bool operator==(const Json &lhs, const Json &rhs) noexcept;
	bool operator!=(const Json &lhs, const Json &rhs) noexcept;
}
#endif	//	JSON_H
