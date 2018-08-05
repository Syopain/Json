#ifndef JSONVALUE_H
#define JSONVALUE_H

#include "json.h"

namespace json{

	class value{
	public:
		int get_type() const noexcept;
		void parse(const std::string &content) noexcept;
	private:

		json::type type_;
	}
}

#endif
