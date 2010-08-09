#include "acsetup.hpp"
#include "xiva/message.hpp"

#include "xiva/channel_info.hpp"

namespace xiva {

message::message()
{
}

message::message(std::string const &value) :
	content_(value)
{
}

message::~message() {
}

std::string const&
message::content() const {
	return content_;
}

void
message::content(std::string const &value) {
	content_.assign(value);
}

channel_info const&
message::get_channel_info() const {
	return ch_info_;
}

void
message::set_channel_info(channel_info const &ch_info) {
	ch_info_ = ch_info;
}


} // namespace
