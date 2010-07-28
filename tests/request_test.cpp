#include "acsetup.hpp"

#include <list>
#include <string>
#include <vector>
#include <sstream>

#include <boost/type.hpp>
#include <boost/mpl/list.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/test_case_template.hpp>

#include "test_utils.hpp"
#include "xiva/http_error.hpp"
#include "details/request_impl.hpp"

namespace xiva { namespace tests {

using namespace details;

typedef boost::mpl::list<std::vector<char>,
std::list<char>, std::string> request_test_types;

BOOST_AUTO_TEST_SUITE(request_test)

bool
is_bad_method(http_error const &e) {
	return http_error::method_not_allowed == e.code();
}

bool
is_bad_protocol_version(http_error const &e) {
	return http_error::version_not_supported == e.code();
}

BOOST_AUTO_TEST_CASE_TEMPLATE(test_get, Range, request_test_types) {

	std::stringstream stream;
	stream << "GET /test?id=1;reason=test HTTP/1.1\r\n";
	stream << "\r\n";
	std::string content = stream.str();

	Range range(content.begin(), content.end());
	request_impl req(range.begin(), range.end());
	BOOST_CHECK_EQUAL("1", req.param("id"));
	BOOST_CHECK_EQUAL("test", req.param("reason"));

}

BOOST_AUTO_TEST_CASE_TEMPLATE(test_cookies, Range, request_test_types) {

	std::stringstream stream;
	stream << "GET / HTTP/1.1\r\n";
	stream << "Cookie: reason=test; id=1\r\n";
	stream << "\r\n";
	std::string content = stream.str();

	Range range(content.begin(), content.end());
	request_impl req(range.begin(), range.end());
	BOOST_CHECK_EQUAL("1", req.cookie("id"));
	BOOST_CHECK_EQUAL("test", req.cookie("reason"));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(test_complex_get, Range, request_test_types) {

	std::stringstream stream;
	stream << "get /test?id=1;reason=test http/1.1\r\n";
	stream << "accept: \r\n";
	stream << " text/css;\r\n";
	stream << " text/html\r\n";
	stream << "\r\n";
	std::string content = stream.str();

	Range range(content.begin(), content.end());
	request_impl req(range.begin(), range.end());
	BOOST_CHECK_EQUAL("text/css; text/html", req.header("accept"));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(test_bad_method, Range, request_test_types) {

	std::stringstream stream;
	stream << "post /test?id=1;reason=test http/1.1\r\n";
	stream << "\r\n";
	std::string content = stream.str();

	Range range(content.begin(), content.end());
	BOOST_CHECK_EXCEPTION(request_impl(range.begin(), range.end()), http_error, is_bad_method);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(test_bad_protocol_version, Range, request_test_types) {

	std::stringstream stream;
	stream << "get /test?id=1;reason=test http/0.9\r\n";
	stream << "\r\n";
	std::string content = stream.str();

	Range range(content.begin(), content.end());
	BOOST_CHECK_EXCEPTION(request_impl(range.begin(), range.end()), http_error, is_bad_protocol_version);
}

BOOST_AUTO_TEST_SUITE_END()

}} // namespaces
