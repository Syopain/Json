#include <assert.h>
#include "jsonException.h"
#include "jsonParser.h"

namespace json {

	inline void expect(const char * &c, char ch)
	{
		assert(*c == ch);
		++c;
	}

	Parser::Parser(Value &v, const std::string &content)
		: val_(v), cur_(content.c_str())
	{
		parse_whitespace();
		parse_value();
	}

	void Parser::parse_value()
	{
		switch (*cur_) {
			case 'n' : parse_null(); return;
			case '\0': throw(Exception("parse expect value"));
			default  : throw(Exception("parse invalid value")); 
		}
	}

	void Parser::parse_whitespace() noexcept 
	{
		while (*cur_ == ' ' || *cur_ == '\t' || *cur_ == '\n' || *cur_ == '\r')
		++cur_;
	}

	void Parser::parse_null()
	{
		expect(cur_, 'n');
		if(cur_[0] != 'u' || cur_[1] != 'l' || cur_[2] != 'l')
			throw (Exception("parse invalid value"));
		cur_ += 3;
		val_.set_type(JSON_NULL);
		return;
	}
}
