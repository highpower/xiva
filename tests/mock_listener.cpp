#include "acsetup.hpp"
#include <stdexcept>
#include "mock_listener.hpp"
#include <boost/lexical_cast.hpp>

namespace xiva { namespace tests {

mock_listener::mock_listener()
{
}

mock_listener::~mock_listener() {
}

bool
mock_listener::available(std::string const &to) const {
	return available_.find(to) != available_.end();
}

void
mock_listener::connection_opened(std::string const &to, globals::connection_id const &id) throw (std::exception) {
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
mock_listener::connection_closed(std::string const &to, globals::connection_id const &id) throw (std::exception) {
	std::map<std::string, connection_ids>::iterator i = available_.find(to);
	if (available_.end() == i) {
		return;
	}
	connection_ids &ids = i->second;
	connection_ids::iterator it = ids.find(id);
	if (ids.end() == it) {
		std::string msg = "wrong connection id: " + boost::lexical_cast<std::string>(id) + " to: " + to;
		throw std::logic_error(msg);
	}
	ids.erase(it);
	if (ids.empty()) {
		available_.erase(i);
	}
}

}} // namespaces
