#include "acsetup.hpp"

#include <ctime>
#include <sstream>
#include <boost/test/unit_test.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/conversion.hpp>
#include "details/http.hpp"


namespace xiva { namespace tests {

using namespace details;

BOOST_AUTO_TEST_SUITE(date_test)

BOOST_AUTO_TEST_CASE(test_format) {

	std::stringstream stream;
	boost::posix_time::ptime now = boost::posix_time::from_time_t(static_cast<time_t>(1170416178));
	
	stream << http_date(now);
	BOOST_CHECK_EQUAL("Date: Fri, 02 Feb 2007 11:36:18 GMT\r\n", stream.str());

}

BOOST_AUTO_TEST_SUITE_END()

}} // namespaces
