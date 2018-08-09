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
}

static void test_parse_invalid_unicode_surrogate() {
    TEST_ERROR("parse invalid unicode surrogate", "\"\\uD800\"");
    TEST_ERROR("parse invalid unicode surrogate", "\"\\uDBFF\"");
    TEST_ERROR("parse invalid unicode surrogate", "\"\\uD800\\\\\"");
    TEST_ERROR("parse invalid unicode surrogate", "\"\\uD800\\uDBFF\"");
    TEST_ERROR("parse invalid unicode surrogate", "\"\\uD800\\uE000\"");
}

static void test_parse() {
	test_parse_literal();
	test_parse_number();
	test_parse_string();
	test_parse_expect_value();
	test_parse_invalid_value();
	test_parse_root_not_singular();
	test_parse_number_too_big();
    test_parse_missing_quotation_mark();
	test_parse_invalid_string_escape();
	test_parse_invalid_string_char();
    test_parse_invalid_unicode_hex();
    test_parse_invalid_unicode_surrogate();
}

int main() {
	test_parse();
	printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
	return main_ret;
}
