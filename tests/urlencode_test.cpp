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
#include "details/urlencode.hpp"

namespace xiva { namespace tests {

using namespace details;

typedef boost::mpl::list<std::vector<char>, std::list<char>,
range<char const*>, std::string> urlencode_test_types;

BOOST_AUTO_TEST_SUITE(urlencode_test)

BOOST_AUTO_TEST_CASE_TEMPLATE(test_urlencode, Range, urlencode_test_types) {
	Range value = as<Range>("раз два три четыре пять");
	BOOST_CHECK_EQUAL("%D2%C1%DA%20%C4%D7%C1%20%D4%D2%C9%20%DE%C5%D4%D9%D2%C5%20%D0%D1%D4%D8", urlencode<std::string>(value));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(test_urlencode_empty, Range, urlencode_test_types) {
	BOOST_CHECK_EQUAL("", urlencode<std::string>(Range()));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(test_urlencode_latin, Range, urlencode_test_types) {
	Range value = as<Range>("abcd efgh");
	BOOST_CHECK_EQUAL("abcd%20efgh", urlencode<std::string>(value));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(test_urldecode, Range, urlencode_test_types) {
	Range value = as<Range>("%D2%C1%DA%20%C4%D7%C1%20%D4%D2%C9%20%DE%C5%D4%D9%D2%C5%20%D0%D1%D4%D8");
	BOOST_CHECK_EQUAL("раз два три четыре пять", urldecode<std::string>(value));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(test_urldecode_empty, Range, urlencode_test_types) {
	BOOST_CHECK_EQUAL("", urldecode<std::string>(Range()));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(test_urldecode_latin, Range, urlencode_test_types) {
	Range value = as<Range>("abcd%20efgh");
	BOOST_CHECK_EQUAL("abcd efgh", urldecode<std::string>(value));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(test_urldecode_bad_suffix, Range, urlencode_test_types) {
	Range value = as<Range>("abcd%20efgh%");
	BOOST_CHECK_EQUAL("abcd efgh%", urldecode<std::string>(value));
}

BOOST_AUTO_TEST_SUITE_END()

}} // namespaces
