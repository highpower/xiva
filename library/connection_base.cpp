#include "acsetup.hpp"
#include "details/connection_base.hpp"

#include <boost/static_assert.hpp>

#include "xiva/http_error.hpp"

#include "details/http_constants.hpp"
#include "details/http.hpp"

namespace xiva { namespace details {

connection_base::connection_base()
{
}

connection_base::~connection_base() {
}

globals::connection_id
connection_base::id() const {
	BOOST_STATIC_ASSERT(sizeof(globals::connection_id) == sizeof(connection_base const*));
	return reinterpret_cast<globals::connection_id>(this);
}

void
connection_base::init(request_impl const &req) {
	ws_info_.parse(req);
}

bool
connection_base::print_policy_data(std::string const &data, std::streambuf &buf) {

	if (!data.empty()) {
		return false;
	}

	std::ostream stream(&buf);
	stream << data << '\0';
	return true;
}

bool
connection_base::print_error(std::streambuf &buf, http_error const &http) const {

	if (!ws_info_.empty()) {
		return false;
	}

	std::ostream stream(&buf);
	stream << http_status(http.code());
	stream << http_date(boost::posix_time::second_clock::universal_time());
	stream << http_header::server();
	stream << http_header::connection_close();
	stream << http_header("Content-Type", "text/plain; charset=utf-8");
	stream << http_constants<char>::endl;
	return true;
}

bool
connection_base::print_headers(std::string const &content_type, std::streambuf &buf) const {

	bool is_websocket = !ws_info_.empty();
	if (is_websocket && !ws_info_.valid()) {
		return false;
	}

	std::ostream stream(&buf);
	if (is_websocket) {
		stream << ws_info_;
	}
	else {
		stream << http_status(200);
		stream << http_header::connection_close();
		stream << http_header("Transfer-Encoding", "chunked");
	}
	stream << http_date(boost::posix_time::second_clock::universal_time());
	stream << http_header::server();
	stream << http_header("Content-Type", content_type.c_str());
	stream << http_constants<char>::endl;
	return true;
}

bool
connection_base::print_static_content(
	std::string const &content_type, std::string const &content, std::streambuf &buf) const {

	if (!ws_info_.empty()) {
		return false;
	}

	std::ostream stream(&buf);

	stream << http_status(200);
	stream << http_header::connection_close();
	stream << http_date(boost::posix_time::second_clock::universal_time());
	stream << http_header::server();
	stream << http_header("Content-Type", content_type.c_str());
	stream << "Content-Length: " << content.size() << http_constants<char>::endl;
	stream << http_constants<char>::endl;
	stream << content;
	return true;
}

bool
connection_base::print_message_content(std::string const &content, std::streambuf &buf) const {

	if (content.empty()) {
		return false;
	}

	std::ostream stream(&buf);

	if (!ws_info_.empty()) {
		ws_info_.write_message(stream, content);
	}
	else {
		stream << std::hex << content.size() << http_constants<char>::endl;
		stream << content << http_constants<char>::endl;
	}
	return true;
}

bool
connection_base::print_last_message(std::streambuf &buf) const {

	if (!ws_info_.empty()) {
		return false;
	}

	std::ostream stream(&buf);

	stream << 0 << http_constants<char>::endl;
	return true;
}

}} // namespaces
