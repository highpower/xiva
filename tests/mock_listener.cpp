#include "acsetup.hpp"
#include "mock_listener.hpp"

#include <stdexcept>
#include <boost/lexical_cast.hpp>

#include "xiva/error.hpp"

namespace xiva { namespace tests {

mock_listener::mock_listener() {
}

mock_listener::~mock_listener() {
}

bool
mock_listener::available(std::string const &to) const {
	return available_.find(to) != available_.end();
}

void
mock_listener::connection_opened(std::string const &to, globals::connection_id id) throw (std::exception) {
	std::map<std::string, connection_ids>::iterator i = available_.find(to);
	if (available_.end() == i) {
		connection_ids ids;
		ids.insert(id);
		ids.swap(available_[to]);
	}
	else {
		connection_ids &ids = i->second;
		ids.insert(id);
	}
}

void
mock_listener::connection_closed(std::string const &to, globals::connection_id id) throw (std::exception) {
	std::map<std::string, connection_ids>::iterator i = available_.find(to);
	if (available_.end() == i) {
		throw error("empty container, failed to close inexistent connection to %s, id %s",
			to.c_str(), boost::lexical_cast<std::string>(id).c_str());
	}
	connection_ids &ids = i->second;
	connection_ids::iterator it = ids.find(id);
	if (ids.end() == it) {
		throw error("non-empty container, failed to close inexistent connection to %s, id %s",
			to.c_str(), boost::lexical_cast<std::string>(id).c_str());
	}
	ids.erase(it);
	if (ids.empty()) {
		available_.erase(i);
	}
}

void
mock_listener::disconnected(std::string const &to) throw (std::exception) {
	(void) to;
	throw error("disconnected");
}

}} // namespaces
