#include "acsetup.hpp"
#include "details/response_impl.hpp"

namespace xiva { namespace details {

response_impl::response_impl() : single_message_(false)
{
}

response_impl::~response_impl() {
}

void
response_impl::swap(response_impl &other) throw () {
	std::swap(type_, other.type_);
	std::swap(formatter_id_, other.formatter_id_);
	std::swap(single_message_, other.single_message_);
}


}} // namespaces
