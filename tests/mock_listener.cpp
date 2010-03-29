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
mock_listener::connection_opened(std::string const &to) throw (std::exception) {
	std::map<std::string, std::size_t>::iterator i = available_.find(to);
	if (available_.end() == i) {
		available_.insert(std::make_pair<std::string, std::size_t>(to, 1));
	}
	else {
		++(i->second);
	}
}

void
mock_listener::connection_closed(std::string const &to) throw (std::exception) {
	std::map<std::string, std::size_t>::iterator i = available_.find(to);
	if (available_.end() == i) {
		throw error("failed to close inexistent connection to %s", to.c_str());
	}
	if (0 == --(i->second)) {
		available_.erase(i);
	}
}

}} // namespaces
