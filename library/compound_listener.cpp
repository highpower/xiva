#include "acsetup.hpp"
#include "details/compound_listener.hpp"

#include <cassert>
#include <algorithm>
#include <exception>
#include <boost/ref.hpp>
#include <boost/bind.hpp>
#include <boost/current_function.hpp>

#include "xiva/logger.hpp"

namespace xiva { namespace details {

compound_listener::compound_listener() {
}

compound_listener::~compound_listener() {
}

void
compound_listener::connection_opened(std::string const &to, globals::connection_id id) throw (std::exception) {
	notify_connection_opened(to, id);
}

void
compound_listener::connection_closed(std::string const &to, globals::connection_id id) throw (std::exception) {
	notify_connection_closed(to, id);
}

void
compound_listener::init(settings const &s) {
	(void) s;
}

void
compound_listener::attach_logger(boost::intrusive_ptr<logger> const &log) {
	assert(log);
	logger_ = log;
}

void
compound_listener::add_connection_listener(listener_ptr_type const &l) {
	assert(l);
	listeners_.push_back(l);
}

bool
compound_listener::empty() const {
	return listeners_.empty();
}

boost::intrusive_ptr<logger> const&
compound_listener::log() const {
	return logger_;
}

void
compound_listener::notify_connection_opened(std::string const &to, globals::connection_id id) {
	try {
		std::for_each(listeners_.begin(), listeners_.end(), 
		    boost::bind(&connection_listener::connection_opened, _1, boost::cref(to), id));
	}
	catch (std::exception const &e) {
		logger_->error("exception caught in %s: %s", BOOST_CURRENT_FUNCTION, e.what());
		throw;
	}
}

void
compound_listener::notify_connection_closed(std::string const &to, globals::connection_id id) {
	try {
		std::for_each(listeners_.begin(), listeners_.end(), 
		    boost::bind(&connection_listener::connection_closed, _1, boost::cref(to), id));
	}
	catch (std::exception const &e) {
		logger_->error("exception caught in %s: %s", BOOST_CURRENT_FUNCTION, e.what());
		throw;
	}
}

}} // namespaces
