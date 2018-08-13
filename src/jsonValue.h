#ifndef JSONVALUE_H
#define JSONVALUE_H

#include <string>
#include <vector>
#include "json.h"

namespace json {

	class Value{
	public:
		void parse(const std::string &content);
		int get_type() const noexcept;
		void set_type(type t) noexcept;
		double get_number() const noexcept;
		void set_number(double d) noexcept;
		const std::string get_string() const noexcept;
		void set_string(const std::string &str) noexcept;
		size_t get_array_size() const noexcept;
		const Value& get_array_element(size_t index) const noexcept;
		void set_array(const std::vector<Value> &arr) noexcept;


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
		};
	};
}

#endif	//	JSONVALUE_H
