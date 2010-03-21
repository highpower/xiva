#include "acsetup.hpp"

#include <boost/test/unit_test.hpp>

#include "xiva/http_error.hpp" 
#include "details/url_matcher.hpp"

namespace xiva { namespace tests {

bool
is_right_http_code(http_error const &http) {
	return http.code() == http_error::not_found;
}

BOOST_AUTO_TEST_SUITE(matcher_test)

BOOST_AUTO_TEST_CASE(test_matcher) {
	
	using namespace details;
	url_matcher matcher;
	
	BOOST_CHECK_EQUAL("swan", matcher.receiver("/swan"));
	BOOST_CHECK_EQUAL("swan", matcher.receiver("/swan?id=123"));
	BOOST_CHECK_EQUAL("swan swan", matcher.receiver("/swan%20swan"));
	BOOST_CHECK_EXCEPTION(matcher.receiver(""), http_error, is_right_http_code);
	BOOST_CHECK_EXCEPTION(matcher.receiver("/swan/"), http_error, is_right_http_code);
	
}

BOOST_AUTO_TEST_SUITE_END()

}} // namespaces
