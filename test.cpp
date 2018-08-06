#include <iostream>
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

static void test_parse_null()
{
	pain::Json v;
	v.set_type(json::False);
	v.parse("null", status);
	EXPECT_EQ_BASE("parse ok", status);
	EXPECT_EQ_BASE(json::Null, v.get_type());
}

static void test_parse_true()
{
	pain::Json v;
	v.parse("true", status);
	EXPECT_EQ_BASE("parse ok", status);
	EXPECT_EQ_BASE(json::True, v.get_type());
}

static void test_parse_false()
{
	pain::Json v;
	v.parse("false", status);
	EXPECT_EQ_BASE("parse ok", status);
	EXPECT_EQ_BASE(json::False, v.get_type());
}

static void test_parse_expect_value()
{
	pain::Json v;

	v.set_type(json::False);
	v.parse("", status);
	EXPECT_EQ_BASE("parse expect value", status);
	EXPECT_EQ_BASE(json::Null, v.get_type());

	v.set_type(json::False);
	v.parse(" ", status);
	EXPECT_EQ_BASE("parse expect value", status);
	EXPECT_EQ_BASE(json::Null, v.get_type());
}

static void test_parse_invalid_value()
{
	pain::Json v;

	v.set_type(json::False);
	v.parse("nul", status);
	EXPECT_EQ_BASE("parse invalid value", status);
	EXPECT_EQ_BASE(json::Null, v.get_type());

	v.set_type(json::False);
	v.parse("?", status);
	EXPECT_EQ_BASE("parse invalid value", status);
	EXPECT_EQ_BASE(json::Null, v.get_type());
}

static void test_parse_root_not_singular()
{
	pain::Json v;

	v.set_type(json::False);
	v.parse("null x", status);
	EXPECT_EQ_BASE("parse root not singular", status);
	EXPECT_EQ_BASE(json::Null, v.get_type());
}

static void test_parse() {
	test_parse_null();
	test_parse_true();
	test_parse_false();
	test_parse_expect_value();
	test_parse_invalid_value();
	test_parse_root_not_singular();
}

int main() {
	test_parse();
	printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
	return main_ret;
}
