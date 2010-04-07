#include "acsetup.hpp"
#include "details/response_impl.hpp"

namespace xiva { namespace details {

response_impl::response_impl() : content_ptr_(NULL), single_message_(false)
{
}

response_impl::~response_impl() {
}

void
response_impl::swap(response_impl &other) throw () {
	std::swap(type_, other.type_);
	std::swap(formatter_id_, other.formatter_id_);
	std::swap(content_, other.content_);
	std::swap(content_ptr_, other.content_ptr_);
	std::swap(single_message_, other.single_message_);
}


}} // namespaces
