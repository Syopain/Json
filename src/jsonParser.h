#ifndef PARSER_H
#define PARSER_H

#include "json.h"
#include "jsonValue.h"

namespace json {
	
	class Parser{
	public:
		Parser(value &v, const std::string &c) : cur_(c.str()) { }
		jsonValue parse();
		
	private:
		void parse_whitespace();
		void parse_null();
		void parse_value();



		char *cur;
		jsonValue const *value

	};

}

#endif	//	PARSER_H
