#include <iostream>
#include <string.h>
#include "src/json.h"

static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;
static std::string status;

using namespace syo;

#define EXPECT_EQ_BASE(expect, actual) \
	do {\
		++test_count;\
		if (expect == actual)\
			++test_pass;\
		else {\
			std::cerr << __FILE__ << ":" << __LINE__ << ": expect: " << expect << " actual: " << actual << std::endl;\
			main_ret = 1;\
		}\
	} while(0)

#define TEST_LITERAL(expect, content)\
	do {\
        syo::Json v;\
		v.set_boolean(false);\
		v.parse(content, status);\
		EXPECT_EQ_BASE("parse ok", status);\
		EXPECT_EQ_BASE(expect, v.get_type());\
	} while (0)

static void test_parse_literal()
{
	TEST_LITERAL(json::Null, "null");
	TEST_LITERAL(json::True, "true");
	TEST_LITERAL(json::False, "false");
}

#define TEST_NUMBER(expect, content)\
	do {\
        syo::Json j;\
		j.parse(content, status);\
		EXPECT_EQ_BASE("parse ok", status);\
		EXPECT_EQ_BASE(json::Number, j.get_type());\
		EXPECT_EQ_BASE(expect, j.get_number());\
	} while(0)

static void test_parse_number()
{
	TEST_NUMBER(0.0, "0");
	TEST_NUMBER(0.0, "-0");
	TEST_NUMBER(0.0, "-0.0");
	TEST_NUMBER(1.0, "1");
	TEST_NUMBER(-1.0, "-1");
	TEST_NUMBER(1.5, "1.5");
	TEST_NUMBER(-1.5, "-1.5");
	TEST_NUMBER(3.1416, "3.1416");
	TEST_NUMBER(1E10, "1E10");
	TEST_NUMBER(1e10, "1e10");
	TEST_NUMBER(1E+10, "1E+10");
	TEST_NUMBER(1E-10, "1E-10");
	TEST_NUMBER(-1E10, "-1E10");
	TEST_NUMBER(-1e10, "-1e10");
	TEST_NUMBER(-1E+10, "-1E+10");
	TEST_NUMBER(-1E-10, "-1E-10");
	TEST_NUMBER(1.234E+10, "1.234E+10");
	TEST_NUMBER(1.234E-10, "1.234E-10");
	TEST_NUMBER(0.0, "1e-10000"); /* must underflow */
}

#define TEST_STRING(expect, content)\
	do {\
        syo::Json v;\
		v.parse(content, status);\
		EXPECT_EQ_BASE("parse ok", status);\
		EXPECT_EQ_BASE(json::String, v.get_type());\
		EXPECT_EQ_BASE(0, memcmp(expect, v.get_string().c_str(), v.get_string().size()));\
	} while(0)

#define EXPECT_EQ_STRING(expect, actual) EXPECT_EQ_BASE(0, memcmp(expect, actual.c_str(), actual.size()));

static void test_parse_string()
{

	TEST_STRING("", "\"\"");
	TEST_STRING("Hello", "\"Hello\"");

	TEST_STRING("Hello\nWorld", "\"Hello\\nWorld\"");
	TEST_STRING("\" \\ / \b \f \n \r \t", "\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\"");

	TEST_STRING("Hello\0World", "\"Hello\\u0000World\"");
	TEST_STRING("\x24", "\"\\u0024\"");         /* Dollar sign U+0024 */
	TEST_STRING("\xC2\xA2", "\"\\u00A2\"");     /* Cents sign U+00A2 */
	TEST_STRING("\xE2\x82\xAC", "\"\\u20AC\""); /* Euro sign U+20AC */
	TEST_STRING("\xF0\x9D\x84\x9E", "\"\\uD834\\uDD1E\"");  /* G clef sign U+1D11E */
	TEST_STRING("\xF0\x9D\x84\x9E", "\"\\ud834\\udd1e\"");  /* G clef sign U+1D11E */
}

static void test_parse_array()
{
    syo::Json v;

	v.parse("[ ]", status);
	EXPECT_EQ_BASE("parse ok", status);
	EXPECT_EQ_BASE(json::Array, v.get_type());
	EXPECT_EQ_BASE(0, v.get_array_size());

	v.parse("[ null , false , true , 123 , \"abc\" ]", status);
	EXPECT_EQ_BASE("parse ok", status);
	EXPECT_EQ_BASE(json::Array, v.get_type());
	EXPECT_EQ_BASE(5, v.get_array_size());
	EXPECT_EQ_BASE(json::Null,   v.get_array_element(0).get_type());
	EXPECT_EQ_BASE(json::False,  v.get_array_element(1).get_type());
	EXPECT_EQ_BASE(json::True,   v.get_array_element(2).get_type());
	EXPECT_EQ_BASE(json::Number, v.get_array_element(3).get_type());
	EXPECT_EQ_BASE(json::String, v.get_array_element(4).get_type());
	EXPECT_EQ_BASE(123.0, v.get_array_element(3).get_number());
	EXPECT_EQ_STRING("abc", v.get_array_element(4).get_string());
	
	v.parse("[ [ ] , [ 0 ] , [ 0 , 1 ] , [ 0 , 1 , 2 ] ]", status);
	EXPECT_EQ_BASE("parse ok", status);
	EXPECT_EQ_BASE(json::Array, v.get_type());
	EXPECT_EQ_BASE(4, v.get_array_size());
	for(int i = 0; i < 4; ++i) {
        syo::Json a = v.get_array_element(i);
		EXPECT_EQ_BASE(json::Array, a.get_type());
		EXPECT_EQ_BASE(i, a.get_array_size());
		for(int j = 0; j < i; ++j) {
            syo::Json e = a.get_array_element(j);
			EXPECT_EQ_BASE(json::Number, e.get_type());
			EXPECT_EQ_BASE((double)j, e.get_number());
		
		}
	}
}

static void test_parse_object()
{
    syo::Json v;

	v.parse(" { } ", status);
	EXPECT_EQ_BASE("parse ok", status);
	EXPECT_EQ_BASE(json::Object, v.get_type());
	EXPECT_EQ_BASE(0, v.get_object_size());

    v.parse(" { "
	        "\"n\" : null , "
	        "\"f\" : false , "
	        "\"t\" : true , "
	        "\"i\" : 123 , "
	        "\"s\" : \"abc\", "
	        "\"a\" : [ 1, 2, 3 ],"
	        "\"o\" : { \"1\" : 1, \"2\" : 2, \"3\" : 3 }"
	        " } ", status);
	EXPECT_EQ_BASE("parse ok", status);
	EXPECT_EQ_BASE(7, v.get_object_size());
	EXPECT_EQ_BASE("n", v.get_object_key(0));
	EXPECT_EQ_BASE(json::Null, v.get_object_value(0).get_type());
	EXPECT_EQ_BASE("f", v.get_object_key(1));
	EXPECT_EQ_BASE(json::False, v.get_object_value(1).get_type());
	EXPECT_EQ_BASE("t", v.get_object_key(2));
	EXPECT_EQ_BASE(json::True, v.get_object_value(2).get_type());
	EXPECT_EQ_BASE("i", v.get_object_key(3));
	EXPECT_EQ_BASE(json::Number, v.get_object_value(3).get_type());
	EXPECT_EQ_BASE(123.0, v.get_object_value(3).get_number());
	EXPECT_EQ_BASE("s", v.get_object_key(4));
	EXPECT_EQ_BASE(json::String, v.get_object_value(4).get_type());
	EXPECT_EQ_BASE("abc", v.get_object_value(4).get_string());
	EXPECT_EQ_BASE("a", v.get_object_key(5));
	EXPECT_EQ_BASE(json::Array, v.get_object_value(5).get_type());
	EXPECT_EQ_BASE(3, v.get_object_value(5).get_array_size());
	for (int i = 0; i < 3; ++i) {
        syo::Json e = v.get_object_value(5).get_array_element(i);
		EXPECT_EQ_BASE(json::Number, e.get_type());
		EXPECT_EQ_BASE(i + 1.0, e.get_number());
	}
	EXPECT_EQ_BASE("o", v.get_object_key(6));
	{
        syo::Json o = v.get_object_value(6);
		EXPECT_EQ_BASE(json::Object, o.get_type());
		for(int i = 0; i < 3; ++i) {
            syo::Json ov = o.get_object_value(i);
			EXPECT_EQ_BASE('1' + i, (o.get_object_key(i))[0]);
			EXPECT_EQ_BASE(1, o.get_object_key_length(i));
			EXPECT_EQ_BASE(json::Number, ov.get_type());
			EXPECT_EQ_BASE(i + 1.0, ov.get_number());
		}
	}

}


#define TEST_ERROR(error, content) \
	do {\
        syo::Json v;\
		v.parse(content, status);\
		EXPECT_EQ_BASE(error, status);\
		EXPECT_EQ_BASE((json::Null), v.get_type());\
	} while(0)

static void test_parse_expect_value()
{
	TEST_ERROR("parse expect value", "");
	TEST_ERROR("parse expect value", " ");
}

static void test_parse_invalid_value()
{
	TEST_ERROR("parse invalid value", "nul");
	TEST_ERROR("parse invalid value", "!?");

	TEST_ERROR("parse invalid value", "+0");
	TEST_ERROR("parse invalid value", "+1");
	TEST_ERROR("parse invalid value", ".123"); /* at least one digit before '.' */
	TEST_ERROR("parse invalid value", "1.");   /* at least one digit after '.' */
	TEST_ERROR("parse invalid value", "INF");
	TEST_ERROR("parse invalid value", "inf");
	TEST_ERROR("parse invalid value", "NAN");
	TEST_ERROR("parse invalid value", "nan");

#if 1
	TEST_ERROR("parse invalid value", "[1,]");
	TEST_ERROR("parse invalid value", "[\"a\", nul]");
#endif
}

static void test_parse_root_not_singular()
{
	TEST_ERROR("parse root not singular", "null x");
	TEST_ERROR("parse root not singular", "truead");
	TEST_ERROR("parse root not singular", "\"dsad\"d");
	
	TEST_ERROR("parse root not singular", "0123");
	TEST_ERROR("parse root not singular", "0x0");
	TEST_ERROR("parse root not singular", "0x123");

}

static void test_parse_number_too_big()
{
	TEST_ERROR("parse number too big", "1e309");
	TEST_ERROR("parse number too big", "-1e309");
}

static void test_parse_missing_quotation_mark()
{
	TEST_ERROR("parse miss quotation mark", "\"");
	TEST_ERROR("parse miss quotation mark", "\"abc");
}

static void test_parse_invalid_string_escape()
{
#if 1
	TEST_ERROR("parse invalid string escape", "\"\\v\"");
	TEST_ERROR("parse invalid string escape", "\"\\'\"");
	TEST_ERROR("parse invalid string escape", "\"\\0\"");
	TEST_ERROR("parse invalid string escape", "\"\\x12\"");
#endif
}

static void test_parse_invalid_string_char()
{
#if 1
	TEST_ERROR("parse invalid string char", "\"\x01\"");
	TEST_ERROR("parse invalid string char", "\"\x1F\"");
#endif
}

static void test_parse_invalid_unicode_hex()
{
	TEST_ERROR("parse invalid unicode hex", "\"\\u\"");
	TEST_ERROR("parse invalid unicode hex", "\"\\u0\"");
	TEST_ERROR("parse invalid unicode hex", "\"\\u01\"");
	TEST_ERROR("parse invalid unicode hex", "\"\\u012\"");
	TEST_ERROR("parse invalid unicode hex", "\"\\u/000\"");
	TEST_ERROR("parse invalid unicode hex", "\"\\uG000\"");
	TEST_ERROR("parse invalid unicode hex", "\"\\u0/00\"");
	TEST_ERROR("parse invalid unicode hex", "\"\\u0G00\"");
	TEST_ERROR("parse invalid unicode hex", "\"\\u0/00\"");
	TEST_ERROR("parse invalid unicode hex", "\"\\u00G0\"");
	TEST_ERROR("parse invalid unicode hex", "\"\\u000/\"");
	TEST_ERROR("parse invalid unicode hex", "\"\\u000G\"");
	TEST_ERROR("parse invalid unicode hex", "\"\\u 123\"");
}

static void test_parse_invalid_unicode_surrogate() {
	TEST_ERROR("parse invalid unicode surrogate", "\"\\uD800\"");
	TEST_ERROR("parse invalid unicode surrogate", "\"\\uDBFF\"");
	TEST_ERROR("parse invalid unicode surrogate", "\"\\uD800\\\\\"");
	TEST_ERROR("parse invalid unicode surrogate", "\"\\uD800\\uDBFF\"");
	TEST_ERROR("parse invalid unicode surrogate", "\"\\uD800\\uE000\"");
}

static void test_parse_miss_comma_or_square_bracket() {
#if 1
	TEST_ERROR("parse miss comma or square bracket", "[1");
	TEST_ERROR("parse miss comma or square bracket", "[1}");
	TEST_ERROR("parse miss comma or square bracket", "[1 2");
	TEST_ERROR("parse miss comma or square bracket", "[[]");
#endif
}

static void test_parse_miss_key()
{
	TEST_ERROR("parse miss key", "{:1,");
	TEST_ERROR("parse miss key", "{1:1,");
	TEST_ERROR("parse miss key", "{true:1,");
	TEST_ERROR("parse miss key", "{false:1,");
	TEST_ERROR("parse miss key", "{null:1,");
	TEST_ERROR("parse miss key", "{[]:1,");
	TEST_ERROR("parse miss key", "{{}:1,");
	TEST_ERROR("parse miss key", "{\"a\":1,");
}

static void test_parse_miss_colon()
{
	TEST_ERROR("parse miss colon", "{\"a\"}");
	TEST_ERROR("parse miss colon", "{\"a\",\"b\"}");
}

static void test_parse_miss_comma_or_curly_bracket()
{
	TEST_ERROR("parse miss comma or curly bracket", "{\"a\":1");
	TEST_ERROR("parse miss comma or curly bracket", "{\"a\":1]");
	TEST_ERROR("parse miss comma or curly bracket", "{\"a\":1 \"b\"");
	TEST_ERROR("parse miss comma or curly bracket", "{\"a\":{}");
}

static void test_parse() {
	test_parse_literal();
	test_parse_number();
	test_parse_string();
	test_parse_array();
	test_parse_object();

	test_parse_expect_value();
	test_parse_invalid_value();
	test_parse_root_not_singular();
	test_parse_number_too_big();
	test_parse_missing_quotation_mark();
	test_parse_invalid_string_escape();
	test_parse_invalid_string_char();
	test_parse_invalid_unicode_hex();
	test_parse_invalid_unicode_surrogate();
	test_parse_miss_comma_or_square_bracket();
	test_parse_miss_key();
	test_parse_miss_colon;
	test_parse_miss_comma_or_curly_bracket();
}

#define TEST_ROUNDTRIP(content)\
	do {\
        syo::Json v;\
		v.parse(content, status);\
		EXPECT_EQ_BASE("parse ok", status);\
		v.stringify(status);\
		EXPECT_EQ_BASE(content, status);\
	} while(0)

static void test_stringify_number() {
	TEST_ROUNDTRIP("0");
	TEST_ROUNDTRIP("-0");
	TEST_ROUNDTRIP("1");
	TEST_ROUNDTRIP("-1");
	TEST_ROUNDTRIP("1.5");
	TEST_ROUNDTRIP("-1.5");
	TEST_ROUNDTRIP("3.25");
	TEST_ROUNDTRIP("1e+20");
	TEST_ROUNDTRIP("1.234e+20");
	TEST_ROUNDTRIP("1.234e-20");

	TEST_ROUNDTRIP("1.0000000000000002"); /* the smallest number > 1 */
	TEST_ROUNDTRIP("4.9406564584124654e-324"); /* minimum denormal */
	TEST_ROUNDTRIP("-4.9406564584124654e-324");
	TEST_ROUNDTRIP("2.2250738585072009e-308");  /* Max subnormal double */
	TEST_ROUNDTRIP("-2.2250738585072009e-308");
	TEST_ROUNDTRIP("2.2250738585072014e-308");  /* Min normal positive double */
	TEST_ROUNDTRIP("-2.2250738585072014e-308");
	TEST_ROUNDTRIP("1.7976931348623157e+308");  /* Max double */
	TEST_ROUNDTRIP("-1.7976931348623157e+308");
}

static void test_stringify_string() {
	TEST_ROUNDTRIP("\"\"");
	TEST_ROUNDTRIP("\"Hello\"");
	TEST_ROUNDTRIP("\"Hello\\nWorld\"");
	TEST_ROUNDTRIP("\"\\\" \\\\ / \\b \\f \\n \\r \\t\"");
	TEST_ROUNDTRIP("\"Hello\\u0000World\"");
}

static void test_stringify_array() {
	TEST_ROUNDTRIP("[]");
	TEST_ROUNDTRIP("[null,false,true,123,\"abc\",[1,2,3]]");
}

static void test_stringify_object() {
	TEST_ROUNDTRIP("{}");
	TEST_ROUNDTRIP("{\"n\":null,\"f\":false,\"t\":true,\"i\":123,\"s\":\"abc\",\"a\":[1,2,3],\"o\":{\"1\":1,\"2\":2,\"3\":3}}");
}

static void test_stringify() {
	TEST_ROUNDTRIP("null");
	TEST_ROUNDTRIP("false");
	TEST_ROUNDTRIP("true");
	test_stringify_number();
	test_stringify_string();
	test_stringify_array();
	test_stringify_object();
}

#define TEST_EQUAL(json1, json2, equality)\
	do {\
        syo::Json v1, v2;\
		v1.parse(json1, status);\
		EXPECT_EQ_BASE("parse ok", status);\
		v2.parse(json2, status);\
		EXPECT_EQ_BASE("parse ok", status);\
		EXPECT_EQ_BASE(equality, int(v1 == v2));\
	} while(0)

static void test_equal() {
	TEST_EQUAL("true", "true", 1);
	TEST_EQUAL("true", "false", 0);
	TEST_EQUAL("false", "false", 1);
	TEST_EQUAL("null", "null", 1);
	TEST_EQUAL("null", "0", 0);
	TEST_EQUAL("123", "123", 1);
	TEST_EQUAL("123", "456", 0);
	TEST_EQUAL("\"abc\"", "\"abc\"", 1);
	TEST_EQUAL("\"abc\"", "\"abcd\"", 0);
	TEST_EQUAL("[]", "[]", 1);
	TEST_EQUAL("[]", "null", 0);
	TEST_EQUAL("[1,2,3]", "[1,2,3]", 1);
	TEST_EQUAL("[1,2,3]", "[1,2,3,4]", 0);
	TEST_EQUAL("[[]]", "[[]]", 1);
	TEST_EQUAL("{}", "{}", 1);
	TEST_EQUAL("{}", "null", 0);
	TEST_EQUAL("{}", "[]", 0);
	TEST_EQUAL("{\"a\":1,\"b\":2}", "{\"a\":1,\"b\":2}", 1);
	TEST_EQUAL("{\"a\":1,\"b\":2}", "{\"b\":2,\"a\":1}", 1);
	TEST_EQUAL("{\"a\":1,\"b\":2}", "{\"a\":1,\"b\":3}", 0);
	TEST_EQUAL("{\"a\":1,\"b\":2}", "{\"a\":1,\"b\":2,\"c\":3}", 0);
	TEST_EQUAL("{\"a\":{\"b\":{\"c\":{}}}}", "{\"a\":{\"b\":{\"c\":{}}}}", 1);
	TEST_EQUAL("{\"a\":{\"b\":{\"c\":{}}}}", "{\"a\":{\"b\":{\"c\":[]}}}", 0);
}

static void test_copy() {
    syo::Json v1, v2;
	v1.parse("{\"t\":true,\"f\":false,\"n\":null,\"d\":1.5,\"a\":[1,2,3]}");
	v2 = v1;
	EXPECT_EQ_BASE(1, int(v2 == v1));
}

static void test_move() {
    syo::Json v1, v2, v3;
	v1.parse("{\"t\":true,\"f\":false,\"n\":null,\"d\":1.5,\"a\":[1,2,3]}");
	v2 = v1;
	v3 = std::move(v2);
	EXPECT_EQ_BASE(json::Null, v2.get_type());
	EXPECT_EQ_BASE(1, int(v3 == v1));
}

static void test_swap() {
    syo::Json v1, v2;
	v1.set_string("Hello");
	v2.set_string("World!");
    syo::swap(v1, v2);
	EXPECT_EQ_BASE("World!", v1.get_string());
	EXPECT_EQ_BASE("Hello",  v2.get_string());
}

static void test_access_null()
{
    syo::Json v;
	v.set_string("a");
	v.set_null();
	EXPECT_EQ_BASE(json::Null, v.get_type());
}

static void test_access_boolean()
{
    syo::Json v;
	v.set_string("a");
	v.set_boolean(false);
	EXPECT_EQ_BASE(json::False, v.get_type());
}

static void test_access_number()
{
    syo::Json v;
	v.set_string("a");
	v.set_number(1234.5);
	EXPECT_EQ_BASE(1234.5, v.get_number());
}

static void test_access_string()
{
    syo::Json v;
	v.set_string("");
	EXPECT_EQ_STRING("", v.get_string());
	v.set_string("Hello");
	EXPECT_EQ_STRING("Hello", v.get_string());
}

static void test_access_array()
{
    syo::Json a, e;
	
	for (size_t j = 0; j < 5; j += 5) {
		a.set_array();
		EXPECT_EQ_BASE(0, a.get_array_size());
		for (int i = 0; i < 10; ++i){
			e.set_number(i);
			a.pushback_array_element(e);
		}

		EXPECT_EQ_BASE(10, a.get_array_size());
		for (int i = 0; i < 10; ++i)
			EXPECT_EQ_BASE(static_cast<double>(i), a.get_array_element(i).get_number());
	}

	a.popback_array_element();
	EXPECT_EQ_BASE(9, a.get_array_size());
	for (int i = 0; i < 9; ++i)
		EXPECT_EQ_BASE(static_cast<double>(i), a.get_array_element(i).get_number());

	a.erase_array_element(4, 0);
	EXPECT_EQ_BASE(9, a.get_array_size());
	for (int i = 0; i < 9; ++i)
	   	EXPECT_EQ_BASE(static_cast<double>(i), a.get_array_element(i).get_number());
	
	a.erase_array_element(8, 1);
	EXPECT_EQ_BASE(8, a.get_array_size());
	for (int i = 0; i < 8; ++i)
	   	EXPECT_EQ_BASE(static_cast<double>(i), a.get_array_element(i).get_number());

	a.erase_array_element(0, 2);
	EXPECT_EQ_BASE(6, a.get_array_size());
	for (int i = 0; i < 6; ++i)
	   	EXPECT_EQ_BASE(static_cast<double>(i)+2, a.get_array_element(i).get_number());

	for (int i = 0; i < 2; ++i) {
		e.set_number(i);
		a.insert_array_element(e, i);
	}

	EXPECT_EQ_BASE(8, a.get_array_size());
	for (int i = 0; i < 8; ++i) {
	   	EXPECT_EQ_BASE(static_cast<double>(i), a.get_array_element(i).get_number());
	}

	e.set_string("Hello");
	a.pushback_array_element(e);

	a.clear_array();
	EXPECT_EQ_BASE(0, a.get_array_size());
}

static void test_access_object()
{
    syo::Json o, v;

	for (int j = 0; j <= 5; j += 5) {
		o.set_object();
		EXPECT_EQ_BASE(0, o.get_object_size());	
		for (int i = 0; i < 10; ++i) {
			std::string key = "a";
			key[0] += i;
			v.set_number(i);
			o.set_object_value(key, v);
		}
		EXPECT_EQ_BASE(10, o.get_object_size());
		for (int i = 0; i < 10; ++i) {
			std::string key = "a";
			key[0] += i;
			auto index = o.find_object_index(key);
			EXPECT_EQ_BASE(1, static_cast<int>(index >= 0));
			v = o.get_object_value(index);
			EXPECT_EQ_BASE(static_cast<double>(i), v.get_number());
		}
	}

	auto index = o.find_object_index("j");
	EXPECT_EQ_BASE(1, static_cast<int>(index >= 0));
	o.remove_object_value(index);
	index = o.find_object_index("j");
	EXPECT_EQ_BASE(1, static_cast<int>(index < 0));
	EXPECT_EQ_BASE(9, o.get_object_size());

	index = o.find_object_index("a");
	EXPECT_EQ_BASE(1, static_cast<int>(index >= 0));
	o.remove_object_value(index);
	index = o.find_object_index("a");
	EXPECT_EQ_BASE(1, static_cast<int>(index < 0));
	EXPECT_EQ_BASE(8, o.get_object_size());

	for (int i = 0; i < 8; i++) {
		std::string key = "a";
		key[0] += i + 1;
		EXPECT_EQ_BASE((double)i + 1, o.get_object_value(o.find_object_index(key)).get_number());
	}

	v.set_string("Hello");
	o.set_object_value("World", v);

	index = o.find_object_index("World");
	EXPECT_EQ_BASE(1, static_cast<int>(index >= 0));
	v = o.get_object_value(index);
	EXPECT_EQ_STRING("Hello", v.get_string());

	o.clear_object();
	EXPECT_EQ_BASE(0, o.get_object_size());

}

static void test_access()
{
	test_access_null();
	test_access_boolean();
	test_access_number();
	test_access_string();
	test_access_array();
	test_access_object();
}

int main() {
	test_parse();
	test_stringify();
	test_equal();
	test_copy();
	test_move();
	test_swap();
	test_access();
	printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
	return main_ret;
}
