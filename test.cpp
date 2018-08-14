#include <iostream>
#include <string.h>
#include "src/json.h"

static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;
static std::string status;

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
		pain::Json v;\
		v.set_type(json::False);\
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
		pain::Json j;\
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
		pain::Json v;\
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
	pain::Json v;

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
		pain::Json a = v.get_array_element(i);
		EXPECT_EQ_BASE(json::Array, a.get_type());
		EXPECT_EQ_BASE(i, a.get_array_size());
		for(int j = 0; j < i; ++j) {
			pain::Json e = a.get_array_element(j);
			EXPECT_EQ_BASE(json::Number, e.get_type());
			EXPECT_EQ_BASE((double)j, e.get_number());
		
		}
	}
}

static void test_parse_object()
{
	pain::Json v;

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
		pain::Json e = v.get_object_value(5).get_array_element(i);
		EXPECT_EQ_BASE(json::Number, e.get_type());
		EXPECT_EQ_BASE(i + 1.0, e.get_number());
	}
	EXPECT_EQ_BASE("o", v.get_object_key(6));
	{
		pain::Json o = v.get_object_value(6);
		EXPECT_EQ_BASE(json::Object, o.get_type());
		for(int i = 0; i < 3; ++i) {
			pain::Json ov = o.get_object_value(i);
			EXPECT_EQ_BASE('1' + i, (o.get_object_key(i))[0]);
			EXPECT_EQ_BASE(1, o.get_object_key_length(i));
			EXPECT_EQ_BASE(json::Number, ov.get_type());
			EXPECT_EQ_BASE(i + 1.0, ov.get_number());
		}
	}

}


#define TEST_ERROR(error, content) \
	do {\
		pain::Json v;\
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

int main() {
	test_parse();
	printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
	return main_ret;
}
