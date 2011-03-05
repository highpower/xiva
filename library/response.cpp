#include "acsetup.hpp"
#include "xiva/response.hpp"
#include "details/response_impl.hpp"

namespace xiva {

response::response(details::response_impl &impl) :
	impl_(impl)
{
}

response::~response() {
}

void
response::content_type(char const *type) {
	impl_.content_type(std::string(type));
}

void
response::content_type(std::string const &type) {
	impl_.content_type(type);
}

void
response::formatter_id(std::string const &fmt_id) {
	impl_.formatter_id(fmt_id);
}

void
response::formatter_by_channel(channel_info const &ch_info, std::string const &fmt_id) {
	impl_.formatter_by_channel(ch_info, fmt_id);
}

void
response::set_header(std::string const &name, std::string const &value) {
	impl_.set_header(name, value);
}

void
response::single_message(bool value) {
	impl_.single_message(value);
}

void
response::content(std::string const &body) {
	impl_.content(body);
}

void
response::content(std::string const *body) {
	impl_.content(body);
}

} // namespaces
