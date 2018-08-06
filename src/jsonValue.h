#ifndef JSONVALUE_H
#define JSONVALUE_H

#include "json.h"

namespace json {

	class Value{
	public:
		int get_type() const noexcept;
		int set_type(type t) noexcept;
		void parse(const std::string &content);

	private:

		json::type type_ = Null;
	};
}

#endif
