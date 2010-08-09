#include "acsetup.hpp"

#include <vector>
#include <iostream>
#include <boost/intrusive_ptr.hpp>
#include <boost/test/unit_test.hpp>

#include "mock_logger.hpp"
#include "mock_listener.hpp"
#include "mock_connection.hpp"

#include "xiva/error.hpp"
#include "xiva/message.hpp"
#include "details/compound_listener.hpp"
#include "details/connection_manager.hpp"

namespace xiva { namespace tests {

using namespace details;

typedef boost::intrusive_ptr<mock_connection> connection_ptr_type;
typedef connection_manager<mock_connection> connection_manager_type;

BOOST_AUTO_TEST_SUITE(manager_test)

bool
accept_error(error const &exc) {
	return exc.what() == std::string("same connection");
}

bool disconnected_error(error const &exc) {
	return exc.what() == std::string("disconnected");
}

void
fill_connections(connection_manager_type &m, std::vector<connection_ptr_type> &v, std::size_t count, std::string const &to) {
	for (std::size_t i = 0; i < count; ++i) {
		connection_ptr_type conn(new mock_connection(to));
		v.push_back(conn);
		m.insert_connection(conn);
	}
}

BOOST_AUTO_TEST_CASE(test_add) {

	boost::intrusive_ptr<compound_listener> listener(new compound_listener());
	listener->add_connection_listener(boost::intrusive_ptr<connection_listener>(new mock_listener()));
	connection_manager_type manager(listener);
	manager.attach_logger(boost::intrusive_ptr<logger>(new mock_logger()));

	connection_ptr_type conn(new mock_connection("highpower"));
	manager.insert_connection(conn);
	BOOST_CHECK_EXCEPTION(manager.insert_connection(conn), error, accept_error);
}

BOOST_AUTO_TEST_CASE(test_deletion) {

	boost::intrusive_ptr<compound_listener> listener(new compound_listener());
	listener->add_connection_listener(boost::intrusive_ptr<connection_listener>(new mock_listener()));
	connection_manager_type manager(listener);
	manager.attach_logger(boost::intrusive_ptr<logger>(new mock_logger()));
	std::vector<connection_ptr_type> connections;

	fill_connections(manager, connections, 10, "swan");
	fill_connections(manager, connections, 10, "bobuk");
	fill_connections(manager, connections, 5, "highpower");
	for (std::vector<connection_ptr_type>::const_iterator i = connections.begin(), end = connections.end(); i != end; ++i) {
		manager.remove_connection(*i);
	}
	BOOST_CHECK(manager.empty());
}

BOOST_AUTO_TEST_CASE(test_messaging) {

	boost::intrusive_ptr<compound_listener> listener(new compound_listener());
	listener->add_connection_listener(boost::intrusive_ptr<connection_listener>(new mock_listener()));
	connection_manager_type manager(listener);
	manager.attach_logger(boost::intrusive_ptr<logger>(new mock_logger()));
	std::vector<connection_ptr_type> connections;

	fill_connections(manager, connections, 10, "swan");
	fill_connections(manager, connections, 10, "bobuk");
	fill_connections(manager, connections, 5, "highpower");

	boost::shared_ptr<message> test_msg(new message("test"));
	manager.send("highpower", test_msg);
	for (std::size_t i = 0 ; i < 1000; ++i) {
		manager.send("swan", test_msg);
	}
	for (std::size_t i = 0 ; i < 100500; ++i) {
		manager.send("bobuk", test_msg);
	}
	for (std::vector<connection_ptr_type>::const_iterator i = connections.begin(), end = connections.end(); i != end; ++i) {
		if ((*i)->name() == "swan") {
			BOOST_CHECK_EQUAL(1000, (*i)->message_count());
		}
		else if ((*i)->name() == "bobuk") {
			BOOST_CHECK_EQUAL(100500, (*i)->message_count());
		}
	}
}

BOOST_AUTO_TEST_CASE(test_disconnected) {

	boost::intrusive_ptr<compound_listener> listener(new compound_listener());
	listener->add_connection_listener(boost::intrusive_ptr<connection_listener>(new mock_listener()));
	connection_manager_type manager(listener);
	manager.attach_logger(boost::intrusive_ptr<logger>(new mock_logger()));
	std::vector<connection_ptr_type> connections;

	boost::shared_ptr<message> test_msg(new message("test"));

	// empty manager
	BOOST_CHECK_EXCEPTION(manager.send("compwolf", test_msg), error, disconnected_error);

	// filled manager
	fill_connections(manager, connections, 10, "swan");
	fill_connections(manager, connections, 10, "bobuk");
	BOOST_CHECK_EXCEPTION(manager.send("highpower", test_msg), error, disconnected_error);
}

BOOST_AUTO_TEST_CASE(test_finishing) {

	boost::intrusive_ptr<compound_listener> listener(new compound_listener());
	listener->add_connection_listener(boost::intrusive_ptr<connection_listener>(new mock_listener()));
	connection_manager_type manager(listener);
	manager.attach_logger(boost::intrusive_ptr<logger>(new mock_logger()));
	std::vector<connection_ptr_type> connections;

	fill_connections(manager, connections, 10, "swan");
	fill_connections(manager, connections, 10, "bobuk");
	fill_connections(manager, connections, 5, "highpower");

	for (std::vector<connection_ptr_type>::const_iterator i = connections.begin(), end = connections.end(); i != end; ++i) {
		BOOST_CHECK(!(*i)->finished());
	}
	manager.finish();
	for (std::vector<connection_ptr_type>::const_iterator i = connections.begin(), end = connections.end(); i != end; ++i) {
		BOOST_CHECK((*i)->finished());
	}

}

BOOST_AUTO_TEST_SUITE_END()

}} // namespaces
