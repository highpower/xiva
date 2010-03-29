#include "acsetup.hpp"
#include "details/threaded_connection.hpp"

#include <boost/static_assert.hpp>

namespace xiva { namespace details {

threaded_connection::threaded_connection() {
}

threaded_connection::~threaded_connection() {
}

std::string
threaded_connection::name() const {
	boost::mutex::scoped_lock lock(mutex_);
	return name_;
}

void
threaded_connection::name(std::string const &name) {
	if (&name != &name_ && !name.empty()) {
		boost::mutex::scoped_lock lock(mutex_);
		name_.assign(name);
	}
}

}} // namespaces
