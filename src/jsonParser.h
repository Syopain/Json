#ifndef PARSER_H
#define PARSER_H

#include "json.h"
#include "jsonValue.h"

namespace json {
	
	class Parser{
	public:
		Parser(Value &v, const std::string &content);
		
	private:
		void parse_whitespace() noexcept;
		void parse_null();
		void parse_true();
		void parse_false();
		void parse_value();

		Value &val_;
		const char *cur_;
	};

}

#endif	//	PARSER_H
