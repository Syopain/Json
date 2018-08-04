#include <iostream>
#include "json.h"

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

inline int expect_eq_int(int expect, int actual)
{
	return expect_eq_base(expect == actual, expect, actual);
}

static void test_parse_null()
{
	json v;
	v.type = JSON_TRUE;
	expect_eq_int(PARSE_OK, v.parse("null"));
	expect_eq_int(JSON_NULL, v.type());
}

static void test_pares() {
	test_parse_null();
}

int main() {
	test_parse();
	printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
	return main_ret;
}
