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
		v.set_type(Null);
		parse_whitespace();
		parse_value();
		parse_whitespace();
		if(*cur_ != '\0')
			throw(Exception("parse root not singular"));
	}

	void Parser::parse_value()
	{
		switch (*cur_) {
			case 'n' : parse_null();  return;
			case 't' : parse_true();  return;
			case 'f' : parse_false(); return;
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
		val_.set_type(json::Null);
		return;
	}

	void Parser::parse_true()
	{
		expect(cur_, 't');
		if(cur_[0] != 'r' || cur_[1] != 'u' || cur_[2] != 'e')
			throw (Exception("parse invalid value"));
		cur_ += 3;
		val_.set_type(json::True);
		return;
	}

	void Parser::parse_false()
	{
		expect(cur_, 'f');
		if(cur_[0] != 'a' || cur_[1] != 'l' || cur_[2] != 's' || cur_[3] != 'e')
			throw (Exception("parse invalid value"));
		cur_ += 4;
		val_.set_type(json::False);
		return;
	}
}
