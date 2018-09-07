#ifndef JSON_GENERATOR_H
#define JSON_GENERATOR_H


#include "jsonValue.h"

namespace json {

	class Generator final{
	public:
		Generator(const Value& val, std::string &result);
	private:
		void stringify_value(const Value &v);
		void stringify_string(const std::string &str);

		std::string &res_;
		
	};

}

#endif	//	JSON_GENERATOR_H
