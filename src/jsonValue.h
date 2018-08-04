#ifndef JSONVALUE_H
#define JSONVALUE_H

#include "json.h"

namespace pain{

	class JsonValue{
	public:
		int get_type() const noexcept;
	private:
		json_type type;
	}
}
#include "jsonValue.hpp"
#endif
