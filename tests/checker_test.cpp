#include "acsetup.hpp"

#include <list>
#include <string>
#include <vector>
#include <utility>

#include <boost/type.hpp>
#include <boost/mpl/list.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/test_case_template.hpp>

#include "test_utils.hpp"
#include "details/range.hpp"
#include "details/char_traits.hpp"
#include "details/request_checker.hpp"

namespace xiva { namespace tests {

using namespace details;

typedef boost::mpl::list<std::vector<char>, std::list<char>,
range<char const*>, std::string> checker_test_types;

BOOST_AUTO_TEST_SUITE(checker_test)

BOOST_AUTO_TEST_CASE_TEMPLATE(test_empty, Range, checker_test_types) {

	Range range;
	request_checker checker;

	std::pair<typename Range::iterator, bool> result = checker(range.begin(), range.end());
	BOOST_CHECK(!result.second);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(test_standard, Range, checker_test_types) {

	request_checker checker;
	is_line_end<char> line_end;
	Range range = as<Range>("GET / HTTP/1.1\r\nHost: xiva.yandex.net\r\n\r\n");

	std::pair<typename Range::iterator, bool> result = checker(range.begin(), range.end());
	BOOST_CHECK(line_end(*result.first));
	BOOST_CHECK(result.second);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(test_nonstandard, Range, checker_test_types) {

	request_checker checker;
	is_line_end<char> line_end;
	Range range = as<Range>("GET / HTTP/1.1\nHost: xiva.yandex.net\n\n");

	std::pair<typename Range::iterator, bool> result = checker(range.begin(), range.end());
	BOOST_CHECK(line_end(*result.first));
	BOOST_CHECK(result.second);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(test_noline_breaks, Range, checker_test_types) {

	request_checker checker;
	Range range = as<Range>("GET / HTTP/1.1");

	std::pair<typename Range::iterator, bool> result = checker(range.begin(), range.end());
	BOOST_CHECK(!result.second);
}

BOOST_AUTO_TEST_SUITE_END()

}} // namespaces
