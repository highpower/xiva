#include "acsetup.hpp"
#include "details/connection_base.hpp"

#include <boost/static_assert.hpp>

#include "xiva/http_error.hpp"
#include "xiva/message.hpp"
#include "xiva/message_filter.hpp"

#include "details/formatters_data.hpp"
#include "details/formatters_factory.hpp"
#include "details/http_constants.hpp"
#include "details/http.hpp"

namespace xiva { namespace details {

connection_base::connection_base() : guard_(new guard())
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
connection_base::init(request_impl const &req, bool secure) {
	ws_info_.parse(req, secure);
}

void
connection_base::init_formatters(formatters_factory const &f, request_impl const &req, response_impl const &resp) {
	fmt_data_ = f.create_formatters_data(req, resp);
}

bool
connection_base::allow_message(message const &msg, message_filter const *filter) const {
	if (NULL == fmt_data_.get()) {
		return NULL == filter || filter->allow_message(msg, http_constants<char>::empty_string, NULL);
	}
	return fmt_data_->allow_message(msg, filter);
}

void
connection_base::update_channels_stat(channels_stat_impl &ch_stat, bool add) const {
	if (NULL != fmt_data_.get()) {
		fmt_data_->update_channels_stat(ch_stat, add);
	}
}

formatter const*
connection_base::default_formatter() const {
	if (NULL == fmt_data_.get()) {
		return NULL;
	}
	return fmt_data_->default_formatter();
}

formatter const*
connection_base::find_formatter(message const &msg) const {
	if (NULL == fmt_data_.get()) {
		return NULL;
	}
	return fmt_data_->find_formatter(msg);
}

void
connection_base::notify_message_printed(message const &msg) {
	if (NULL != fmt_data_.get()) {
		fmt_data_->update(msg);
	}
}

bool
connection_base::print_policy_data(std::string const &data, std::streambuf &buf) {

	if (data.empty()) {
		return false;
	}

	std::ostream stream(&buf);
	stream << data << '\0';
	return true;
}

bool
connection_base::print_error(std::streambuf &buf, http_error const &http) const {

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
		ws_info_.write_headers(stream);
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
	if (is_websocket) {
		ws_info_.write_body(stream);
	}
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
