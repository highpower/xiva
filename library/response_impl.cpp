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
	std::swap(channels_data_, other.channels_data_);
}

void
response_impl::formatter_by_channel(channel_info const &ch_info, std::string const &fmt_id) {
	if (!ch_info.empty()) {
		channels_data_[ch_info] = fmt_id;
	}
}


}} // namespaces
