#include "acsetup.hpp"
#include "details/response_impl.hpp"

namespace xiva { namespace details {

response_impl::response_impl()
{
}

response_impl::~response_impl() {
}

void
response_impl::swap(response_impl &other) throw () {
}

std::string const&
response_impl::content_type() const {
	return type_;
}

void
response_impl::content_type(std::string const &type) {
	// TODO check for correctness of type
	type_.assign(type);
}

}} // namespaces
