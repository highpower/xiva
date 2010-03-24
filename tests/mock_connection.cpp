#include "acsetup.hpp"
#include "mock_connection.hpp"

namespace xiva { namespace tests {

mock_connection::mock_connection(std::string const &to) :
	connection(), finished_(false), message_count_(0)
{
	name(to);
}

mock_connection::~mock_connection() {
}

bool
mock_connection::finished() const {
	return finished_;
}

std::size_t
mock_connection::message_count() const {
	return message_count_;
}

void
mock_connection::finish() {
	finished_ = true;
}

void
mock_connection::send(boost::shared_ptr<message> const &m) {
	(void) m;
	++message_count_;
}

void
mock_connection::validate_result(std::string const &to, char const *content_type) {
	(void) to;
	(void) content_type;
}

}} // namespaces
