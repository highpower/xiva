#include "acsetup.hpp"
#include "details/threaded_connection.hpp"

#include <boost/static_assert.hpp>

namespace xiva { namespace details {

threaded_connection::threaded_connection() {
}

threaded_connection::~threaded_connection() {
}

std::string const&
threaded_connection::name() const {
	return name_;
}

void
threaded_connection::name(std::string const &name) {
	name_.assign(name);
}

}} // namespaces
