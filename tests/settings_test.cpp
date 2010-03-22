#include "acsetup.hpp"

#include <sstream>
#include <boost/test/unit_test.hpp>

#include "xml_settings.hpp"

namespace xiva { namespace tests {

BOOST_AUTO_TEST_SUITE(settings_test)

BOOST_AUTO_TEST_CASE(test_value) {
	
	daemon::xml_settings set("test.conf");
	BOOST_CHECK_EQUAL(10, set.as<int>("/" XIVA_PACKAGE_NAME "/endpoint/backlog"));
	BOOST_CHECK_EQUAL("/tmp/" XIVA_PACKAGE_NAME "-1.sock", set.as<std::string>("/" XIVA_PACKAGE_NAME "/endpoint/socket"));
}

BOOST_AUTO_TEST_CASE(test_enumeration) {

	daemon::xml_settings set("test.conf");
	enumeration<std::string>::ptr_type en = set.value_list("/" XIVA_PACKAGE_NAME "/proxy-deny/url");
	for (std::size_t i = 1; !en->empty(); ++i) {
		std::ostringstream stream;
		stream << "http://127.0.0.1/" << i << ".html";
		BOOST_CHECK_EQUAL(stream.str(), en->next());
	}
}

BOOST_AUTO_TEST_SUITE_END();

}} // namespaces
