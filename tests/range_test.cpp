#include "acsetup.hpp"

#include <list>
#include <string>
#include <vector>

#include <boost/type.hpp>
#include <boost/range.hpp>
#include <boost/mpl/list.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/test_case_template.hpp>

#include "test_utils.hpp"
#include "details/range.hpp"

namespace xiva { namespace tests {

using namespace details;

typedef boost::mpl::list<std::vector<char>, std::list<char>, 
	details::range<char const*>, std::string> range_test_types;

BOOST_AUTO_TEST_SUITE(string_test) 

BOOST_AUTO_TEST_CASE_TEMPLATE(test_construct, Range, range_test_types) {

	typedef range<typename Range::const_iterator> test_range;
	
	test_range tr;
	BOOST_CHECK_EQUAL(true, tr.empty());
	BOOST_CHECK_EQUAL(0, tr.size());

	Range r = as<Range>("12345");
	test_range other = make_range(r);

	BOOST_CHECK_EQUAL(false, other.empty());	
	BOOST_CHECK_EQUAL(5, other.size());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(test_comparision, Range, range_test_types) {

	Range r = as<Range>("test string");
	range<char const*> pattern = make_range("test string");
	range<typename Range::const_iterator> result = make_range(r);
	
	BOOST_CHECK_EQUAL(true, result == pattern);
	BOOST_CHECK_EQUAL(true, result <= pattern);
	BOOST_CHECK_EQUAL(true, result >= pattern);
	BOOST_CHECK_EQUAL(false, result != pattern);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(test_reversed_comparision, Range, range_test_types) {

	Range r = as<Range>("test string");
	range<char const*> source = make_range("test string");
	range<typename Range::const_reverse_iterator> result(r.rbegin(), r.rend());
	range<range<char const*>::const_reverse_iterator> pattern(source.rbegin(), source.rend());
	
	BOOST_CHECK_EQUAL(true, result == pattern);
	BOOST_CHECK_EQUAL(true, result <= pattern);
	BOOST_CHECK_EQUAL(true, result >= pattern);
	BOOST_CHECK_EQUAL(false, result != pattern);
}

BOOST_AUTO_TEST_SUITE_END()

}} // namespaces
