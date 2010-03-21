#include "acsetup.hpp"

#include <list>
#include <string>
#include <vector>

#include <boost/type.hpp>
#include <boost/mpl/list.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/test_case_template.hpp>

#include "test_utils.hpp"
#include "details/range.hpp"
#include "details/string_utils.hpp"

namespace xiva { namespace tests {

using namespace details;

typedef boost::mpl::list<std::vector<char>, std::list<char>, 
	range<char const*>, std::string> string_test_types;

BOOST_AUTO_TEST_SUITE(string_test) 

BOOST_AUTO_TEST_CASE_TEMPLATE(test_trim, Range, string_test_types) {

	BOOST_CHECK_EQUAL("", trim(std::string()));
	BOOST_CHECK_EQUAL("test", make_string(trim(as<Range>("  test  "))));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(test_trim_left, Range, string_test_types) {
	BOOST_CHECK_EQUAL("test  ", make_string(trim_left(as<Range>("   test  "))));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(test_trim_right, Range, string_test_types) {
	BOOST_CHECK_EQUAL("  test", make_string(trim_right(as<Range>("  test  "))));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(test_split, Range, string_test_types) {
	
	Range value, head, tail;
	value = as<Range>("best test best test best");
	while (!value.empty()) {
		split_once(value, ' ', head, tail);
		BOOST_CHECK_EQUAL(4, head.size());
		value = trim(tail);
	}
	value = as<Range>("best test best test best");
	while (!value.empty()) {
		split_once(value, "test", head, tail);
		BOOST_CHECK_EQUAL("best", make_string(trim(head)));
		value = trim(tail);
	}
	value = as<Range>("best test best test best");
	while (!value.empty()) {
		typedef range<char const*> range_type;
		split_once(value, as<range_type>("test"), head, tail);
		BOOST_CHECK_EQUAL("best", make_string(trim(head)));
		value = trim(tail);
	}
}

BOOST_AUTO_TEST_CASE_TEMPLATE(test_split_first_of, Range, string_test_types) {
	
	Range value, head, tail;
	value = as<Range>("a&b;c&d;e&f");
	while (!value.empty()) {
		split_first_of_once(value, "&;", head, tail);
		BOOST_CHECK_EQUAL(1, head.size());
		value = trim(tail);
	}
	value = as<Range>("a&b;c&d;e&f");
	while (!value.empty()) {
		typedef range<char const*> range_type;
		split_first_of_once(value, as<range_type>("&;"), head, tail);
		BOOST_CHECK_EQUAL(1, head.size());
		value = trim(tail);
	}
}

BOOST_AUTO_TEST_CASE_TEMPLATE(test_starts_ends, Range, string_test_types) {

	BOOST_CHECK_EQUAL(true, starts_with(as<Range>("testing"), "test"));
	BOOST_CHECK_EQUAL(false, starts_with(as<Range>("production"), "test"));
	BOOST_CHECK_EQUAL(true, starts_with("testing", as<Range>("test")));
	BOOST_CHECK_EQUAL(false, starts_with("production", as<Range>("test")));
	BOOST_CHECK_EQUAL(true, starts_with(as<Range>("testing"), as<Range>("test")));
	BOOST_CHECK_EQUAL(false, starts_with(as<Range>("production"), as<Range>("test")));

	BOOST_CHECK_EQUAL(true, ends_with(as<Range>("testing"), "ing"));
	BOOST_CHECK_EQUAL(false, ends_with(as<Range>("production"), "ing"));
	BOOST_CHECK_EQUAL(true, ends_with("testing", as<Range>("ing")));
	BOOST_CHECK_EQUAL(false, ends_with("production", as<Range>("ing")));
	BOOST_CHECK_EQUAL(true, ends_with(as<Range>("testing"), as<Range>("ing")));
	BOOST_CHECK_EQUAL(false, ends_with(as<Range>("production"), as<Range>("ing")));
}

BOOST_AUTO_TEST_SUITE_END()

}} // namespaces
