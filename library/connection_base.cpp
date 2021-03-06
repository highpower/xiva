#include "acsetup.hpp"
#include "details/connection_base.hpp"

#include <boost/static_assert.hpp>

#include "xiva/formatter.hpp"
#include "xiva/http_error.hpp"
#include "xiva/message.hpp"
#include "xiva/message_filter.hpp"

#include "details/connection_data.hpp"
#include "details/formatters_data.hpp"
#include "details/formatters_factory.hpp"
#include "details/http_constants.hpp"
#include "details/http.hpp"
#include "details/request_impl.hpp"

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
connection_base::init(request_impl &req, bool secure) {
	if (ws_info_.parse(req, secure)) {
		req.set_websocket();
	}
}

void
connection_base::init_formatters(connection_data const &cdata, request_impl const &req, response_impl const &resp) {
	fmt_data_.reset(cdata.fmt_factory().create_formatters_data(req, resp).release());
}

void
connection_base::handle_response(response_impl const &resp) {
	headers_ = resp.headers();
}

bool
connection_base::allow_message(message const &msg, message_filter const *filter) const {
	if (NULL == fmt_data_.get()) {
		return NULL == filter || filter->allow_message(msg, http_constants<char>::empty_string);
	}
	return fmt_data_->allow_message(msg, filter);
}

void
connection_base::update_channels_stat(channels_stat_impl &ch_stat, bool add) const {
	if (NULL != fmt_data_.get()) {
		fmt_data_->update_channels_stat(ch_stat, add);
	}
}

bool
connection_base::print_message(message const &msg, std::streambuf &buf) {

	formatter const *fmt_ptr = NULL;
	if (NULL != fmt_data_.get()) {
		fmt_ptr = fmt_data_->find_formatter(msg);
	}

	std::string const &content = msg.content();
	if (NULL != fmt_ptr) {
		if (!print_message_content(fmt_ptr->wrap_message(content), buf)) {
			return false;
		}
	}
	else if (!print_message_content(content, buf)) {
		return false;
	}

	if (NULL != fmt_data_.get()) {
		fmt_data_->update(msg);
	}
	return true;
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
	print_common_headers(content_type, buf);
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
	print_common_headers(content_type, buf);
	stream << "Content-Length: " << content.size() << http_constants<char>::endl;
	stream << http_constants<char>::endl;
	stream << content;
	return true;
}

void
connection_base::print_common_headers(std::string const &content_type, std::streambuf &buf) const {

	std::ostream stream(&buf);

	stream << http_date(boost::posix_time::second_clock::universal_time());
	stream << http_header::server();
	stream << http_header("Content-Type", content_type.c_str());

	if (headers_ && !headers_->empty()) {
		response_impl::headers_data_type::const_iterator it = headers_->begin(), end = headers_->end();
		for (; it != end; ++it) {
			stream << http_header(it->first.c_str(), it->second.c_str());
		}
	}
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

	std::ostream stream(&buf);

	if (!ws_info_.empty()) {
		return ws_info_.write_close(stream);
	}

	stream << 0 << http_constants<char>::endl;
	return true;
}

}} // namespaces
