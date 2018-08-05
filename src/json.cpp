#include "json.h"
#include "jsonValue.h"

namespace pain 

	Json Json::parse(const string &content) noexcept
	{
		
	}

	Json(Json &src)
	{
		value.reset(new json::value(*src-> v));
	}

	int Json::type() const noexcept
	{
		return v-> get_type();
	}

}
