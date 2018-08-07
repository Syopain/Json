#ifndef JSONVALUE_H
#define JSONVALUE_H

#include "json.h"

namespace json {

	class Value{
	public:
		void parse(const std::string &content);
		int get_type() const noexcept;
		void set_type(type t) noexcept;
		double get_number() const noexcept;
		void set_number(double d) noexcept;

	private:

		json::type type_ = json::Null;
		double num_ = 0;
	};
}

#endif	//	JSONVALUE_H
