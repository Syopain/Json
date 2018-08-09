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
		void parse_value();
		void parse_literal(const char *literal, json::type t);
		void parse_number();
		void parse_string();
		void parse_hex4(const char* &p, unsigned &u);
		void parse_encode_utf8(std::string &s, unsigned u);

		Value &val_;
		const char *cur_;
	};

}

#endif	//	PARSER_H
