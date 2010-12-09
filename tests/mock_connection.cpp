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
mock_connection::close() {
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
mock_connection::handled(details::request_impl const &req, details::response_impl const &resp) {
	(void) req;
	(void) resp;
}

void
mock_connection::handled_errors(
	details::request_impl const &req, details::response_impl const &resp,
	unsigned short http_code, std::string const &error_msg) {

	(void) req;
	(void) resp;
	(void) http_code;
	(void) error_msg;
}
				

}} // namespaces
