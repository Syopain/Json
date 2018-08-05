#include <iostream>
#include "src/json.h"
//#include "src/jsonValue.h"
static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;

template <typename T>
inline void expect_eq_base(bool equality, T expect, T actual)
{
	++test_count;
	if (equality)
		++test_pass;
	else {
		std::cerr << __FILE__ << ":" << __LINE__ << ": expect: " << expect << " actual: " << actual << std::endl;
		main_ret = 1;
	}
}

inline void expect_eq_int(int expect, int actual)
{
	expect_eq_base(expect == actual, expect, actual);
}

static void test_parse_null()
{
	pain::Json v;
	v.set_type(json::JSON_TRUE);
	v.parse("null");
	expect_eq_int(json::JSON_NULL, v.get_type());
}

static void test_parse() {
	test_parse_null();
}

int main() {
	test_parse();
	printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
	return main_ret;
}
