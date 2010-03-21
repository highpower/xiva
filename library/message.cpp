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

} // namespace
