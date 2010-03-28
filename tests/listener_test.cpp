#include "acsetup.hpp"

#include <vector>
#include <boost/intrusive_ptr.hpp>
#include <boost/test/unit_test.hpp>

#include "mock_logger.hpp"
#include "mock_listener.hpp"
#include "details/compound_listener.hpp"

namespace xiva { namespace tests {

using namespace details;

BOOST_AUTO_TEST_SUITE(listener_test)

BOOST_AUTO_TEST_CASE(test_listener) {

	compound_listener compound;
	compound.attach_logger(boost::intrusive_ptr<logger>(new mock_logger()));
	typedef boost::intrusive_ptr<mock_listener> listener_ptr_type;

	std::vector<listener_ptr_type> listeners;
	for (std::size_t i = 0; i < 100; ++i) {
		listener_ptr_type l(new mock_listener());
		compound.add_connection_listener(l);
		listeners.push_back(l);
	}
	compound.connection_opened("swan", 1);
	compound.connection_opened("highpower", 2);
	compound.connection_opened("ponomarev", 3);

	for (std::vector<listener_ptr_type>::const_iterator i = listeners.begin(), end = listeners.end(); i != end; ++i) {
		BOOST_CHECK_EQUAL(true, (*i)->available("swan"));
		BOOST_CHECK_EQUAL(true, (*i)->available("highpower"));
		BOOST_CHECK_EQUAL(true, (*i)->available("ponomarev"));
		BOOST_CHECK_EQUAL(false, (*i)->available("bobuk"));
	}
	compound.connection_closed("highpower", 2);
	for (std::vector<listener_ptr_type>::const_iterator i = listeners.begin(), end = listeners.end(); i != end; ++i) {
		BOOST_CHECK_EQUAL(false, (*i)->available("highpower"));
		BOOST_CHECK_EQUAL(true, (*i)->available("swan"));
	}
	BOOST_CHECK_NO_THROW(compound.connection_closed("highpower", 2));
	BOOST_CHECK_NO_THROW(compound.connection_closed("highpower", 3));
	BOOST_CHECK_THROW(compound.connection_closed("swan", 2), std::exception);
}

BOOST_AUTO_TEST_SUITE_END()

}} // namespaces
