#ifndef JSONVALUE_H
#define JSONVALUE_H

#include <string>
#include <vector>
#include <utility>
#include "json.h"

namespace json {

	class Value{
	public:
		void parse(const std::string &content);
		void stringify(std::string &content) const noexcept;
		int get_type() const noexcept;
		void set_type(type t) noexcept;
		double get_number() const noexcept;
		void set_number(double d) noexcept;
		const std::string& get_string() const noexcept;
		void set_string(const std::string &str) noexcept;
		size_t get_array_size() const noexcept;
		const Value& get_array_element(size_t index) const noexcept;
		void set_array(const std::vector<Value> &arr) noexcept;
		size_t get_object_size() const noexcept;
		const std::string& get_object_key(size_t index) const noexcept;
		size_t get_object_key_length(size_t index) const noexcept;
		const Value& get_object_value(size_t index) const noexcept;
		void set_object(const std::vector<std::pair<std::string, Value>> &obj) noexcept;
		long long find_object_index(const std::string &key) const noexcept;

		Value() noexcept { num_ = 0; }
		Value(const Value &rhs) noexcept { init(rhs); }
		Value& operator=(const Value &rhs) noexcept;
		~Value() noexcept;

	private:
		void init(const Value &rhs) noexcept; 
		void free() noexcept;

		json::type type_ = json::Null;
		union {
			double num_;
			std::string str_;
			std::vector<Value> arr_;
			std::vector<std::pair<std::string, Value>> obj_;
		};
		
		friend bool operator==(const Value &lhs, const Value &rhs) noexcept;
	};
	bool operator==(const Value &lhs, const Value &rhs) noexcept;
	bool operator!=(const Value &lhs, const Value &rhs) noexcept;
}

#endif	//	JSONVALUE_H
