#include "acsetup.hpp"
#include "xiva/message.hpp"

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

std::set<std::string> const&
message::channels() const {
	return channels_;
}

void
message::swap_channels(std::set<std::string> &ch) {
	channels_.swap(ch);
}


} // namespace
