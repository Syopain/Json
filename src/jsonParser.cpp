#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include "jsonException.h"
#include "jsonParser.h"
#include <iostream>
namespace json {

	inline void expect(const char * &c, char ch)
	{
		assert(*c == ch);
		++c;
	}

	Parser::Parser(Value &v, const std::string &content)
		: val_(v), cur_(content.c_str())
	{
		v.set_type(json::Null);
		parse_whitespace();
		parse_value();
		parse_whitespace();
		if(*cur_ != '\0'){
			val_.set_type(json::Null);
			throw(Exception("parse root not singular"));
		}
	}

	void Parser::parse_value()
	{
		switch (*cur_) {
			case 'n' : parse_literal("null", json::Null);  return;
			case 't' : parse_literal("true", json::True);  return;
			case 'f' : parse_literal("false", json::False); return;
			case '\"': parse_string();return;
			default  : parse_number();return; 
			case '\0': throw(Exception("parse expect value"));
		}
	}

	void Parser::parse_whitespace() noexcept 
	{
		while (*cur_ == ' ' || *cur_ == '\t' || *cur_ == '\n' || *cur_ == '\r')
		++cur_;
	}

	void Parser::parse_literal(const char *literal, json::type t)
	{
		expect(cur_, literal[0]);
		size_t i;
		for(i = 0; literal[i+1]; ++i) {
			if (cur_[i] != literal[i+1])
				throw (Exception("parse invalid value"));
		}
		cur_ += i;
		val_.set_type(t);
	}

	void Parser::parse_number()
	{
		
		const char *p = cur_;
		if(*p == '-') ++p;
		if(*p == '0') ++p;
		else {
			if(!isdigit(*p)) throw (Exception("parse invalid value"));
			while(isdigit(*++p)) ;
		}
		if(*p == '.'){
			if(!isdigit(*++p)) throw (Exception("parse invalid value"));
			while(isdigit(*++p)) ;
		}
		if(*p == 'e' || *p == 'E') {
			++p;
			if(*p == '+' || *p == '-') ++p;
			if(!isdigit(*p)) throw (Exception("parse invalid value"));
			while(isdigit(*++p)) ;
		}
		errno = 0;
		double v = strtod(cur_, NULL);
		if (errno == ERANGE && (v == HUGE_VAL || v == -HUGE_VAL))
			throw (Exception("parse number too big"));
		val_.set_number(v);
		cur_ = p;
	}

	void Parser::parse_string()
	{
		expect(cur_, '\"');
		const char *p = cur_;
		std::string tmp;
		while (*p != '\"') {
			if(*p == '\0')
				throw(Exception("parse miss quotation mark"));
			if(*p == '\\' && ++p){
				switch(*p++){
					case '\"': tmp += '\"'; break;
					case '\\': tmp += '\\'; break;
					case '/' : tmp += '/' ; break;
					case 'b' : tmp += '\b'; break;
					case 'f' : tmp += '\f'; break;
					case 'n' : tmp += '\n'; break;
					case 'r' : tmp += '\r'; break;
					case 't' : tmp += '\t'; break;
					default	 : throw (Exception("parse invalid string escape"));
				}
			}
			else if ((unsigned char) *p < 0x20) {
				throw (Exception("parse invalid string char"));
			} else tmp += *p++;
		}
		val_.set_string(tmp);
		cur_ = ++p;
	}
}
