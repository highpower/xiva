#include "acsetup.hpp"
#include "details/connection_impl.hpp"

#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

#include "xiva/logger.hpp"
#include "xiva/message.hpp"
#include "xiva/http_error.hpp"
#include "xiva/receiver_matcher.hpp"

#include "details/http.hpp"
#include "details/request_impl.hpp"
#include "details/header_checker.hpp"
#include "details/connection_data.hpp"
#include "details/connection_manager.hpp"


namespace xiva { namespace details {

connection_impl::connection_impl(asio::io_service &io, boost::intrusive_ptr<logger> const &log, connection_data const &data) :
	io_(io), data_(data), writing_message_(false), logger_(log), timer_(io_), socket_(io_)
{
}

connection_impl::~connection_impl() {

}

void
connection_impl::start() {
	asio::ip::tcp::endpoint endpoint = socket_.remote_endpoint();
	asio::ip::address addr = endpoint.address();
	addr_ = addr.to_string();
	read();
}

void
connection_impl::finish() {
	send(boost::shared_ptr<message>());
}

void
connection_impl::send(boost::shared_ptr<message> const &m) {
	messages_.push_back(m);
	if (!writing_message_) {
		write_message();
	}
}

void
connection_impl::handle_read(syst::error_code const &code) {
 	timer_.cancel();
 	if (code) {
		handle_error(code);
		return;
	}
	boost::intrusive_ptr<connection_impl> self(this);
	try {
		
		typedef streambuf_type::const_buffers_type buffers_type;
		typedef asio::buffers_iterator<buffers_type> iterator_type;
		
		buffers_type const &data = in_.data();
		request_impl req(iterator_type::begin(data), iterator_type::end(data));
		
		boost::intrusive_ptr<receiver_matcher> matcher = data_.matcher();
		boost::intrusive_ptr<connection_manager> manager = data_.manager();
		
		std::string receiver = matcher->receiver(req);
		if (receiver.empty()) {
			throw http_error(http_error::not_found);
		}
		name(receiver);
		manager->insert_connection(self);
		logger_->debug("name %s assigned to connection[%lu] from %s", receiver.c_str(), id(), address());
		
		write_headers(matcher->content_type());
	}
	catch (http_error const &h) {
		write_http_error(h);
	}
	catch (std::exception const &e) {
		handle_exception(e);
	}
}
	
void
connection_impl::handle_write_headers(syst::error_code const &code) {
 	timer_.cancel();
 	if (code) {
		handle_error(code);
		return;
	}
	write_message();
}

void
connection_impl::handle_write_message(syst::error_code const &code) {
	timer_.cancel();
	writing_message_ = false;
	if (code) {
		handle_error(code);
		return;
	}
	messages_.pop_front();
	write_message();
}

void
connection_impl::handle_cleanup(syst::error_code const &code) {
	(void) code;
	cleanup();
}

void
connection_impl::handle_timeout(syst::error_code const &code) {
	if (!code) {
		logger_->info("timeout was reached with connection[%lu] from %s", id(), address());
		cleanup();
	}
}

void
connection_impl::handle_inactive_timeout(syst::error_code const &code) {
	if (!code) {
		logger_->info("inactive timeout was reached with connection[%lu] from %s", id(), address());
		cleanup();
	}
}

void
connection_impl::read() {

	boost::intrusive_ptr<connection_impl> self(this);
	try {
		asio::async_read_until(socket_, in_, header_checker(), 
			boost::bind(&connection_impl::handle_read, self, asio::placeholders::error));
		setup_timeout(data_.read_timeout());
	}
	catch (std::exception const &e) {
		handle_exception(e);
	}
}

void
connection_impl::write_headers(char const *content_type) {

	boost::intrusive_ptr<connection_impl> self(this);
	try {
		print_headers(content_type, out_);
		asio::async_write(socket_, out_,
			boost::bind(&connection_impl::handle_write_headers, self,
			asio::placeholders::error));	
		setup_timeout(data_.write_timeout());
	}
	catch (std::exception const &e) {
		handle_exception(e);
	}
}

void
connection_impl::write_message() {
	
	if (messages_.empty()) {
		setup_inactive_timeout();
		return;
	}
	timer_.cancel();
	boost::shared_ptr<message> const &message = messages_.front();
	if (!message) {
		write_last_message();
		return;
	}
	
	boost::intrusive_ptr<connection_impl> self(this);
	try {
		std::ostream stream(&out_);
		std::string const &content = message->content();
		stream << std::hex << content.size() << http_constants::endl;
		stream << content << http_constants::endl;
		writing_message_ = true;
		asio::async_write(socket_, out_, boost::bind(&connection_impl::handle_write_message,
			self, asio::placeholders::error));
		setup_timeout(data_.write_timeout());
	}
	catch (std::exception const &e) {
		handle_exception(e);
	}
}

void
connection_impl::write_last_message() {
	boost::intrusive_ptr<connection_impl> self(this);
	try {
		std::ostream stream(&out_);
		stream << 0 << http_constants::endl;
		writing_message_ = true;
		asio::async_write(socket_, out_, boost::bind(&connection_impl::handle_cleanup,
			self, asio::placeholders::error));
		setup_timeout(data_.write_timeout());
	}
	catch (std::exception const &e) {
		handle_exception(e);
	}
}

void
connection_impl::write_http_error(http_error const &http) {

	logger_->error("http error occured with connection[%lu] from %s: %s", id(), address(), http.what());
	boost::intrusive_ptr<connection_impl> self(this);
	try {
		print_error(out_, http);
		std::ostream stream(&out_);
		asio::async_write(socket_, out_, boost::bind(&connection_impl::handle_cleanup, 
			self, asio::placeholders::error));
		setup_timeout(data_.write_timeout());
	}
	catch (std::exception const &e) {
		handle_exception(e);
	}
}

void
connection_impl::cleanup() {
	timer_.cancel();
	std::string const &n = name();
	if (!n.empty()) {
		boost::intrusive_ptr<connection_impl> self(this);
		boost::intrusive_ptr<connection_manager> manager = data_.manager();
		manager->remove_connection(self);
	}
	socket_.close();
	logger_->info("connection[%lu] from %s is closed", id(), address());
}

void
connection_impl::setup_timeout(unsigned short timeout) {
	
	boost::intrusive_ptr<connection_impl> self(this);
	timer_.expires_from_now(boost::posix_time::milliseconds(timeout));
	timer_.async_wait(boost::bind(&connection_impl::handle_timeout, self, 
		asio::placeholders::error));
}

void
connection_impl::setup_inactive_timeout() {
	
	boost::intrusive_ptr<connection_impl> self(this);
	timer_.expires_from_now(boost::posix_time::milliseconds(data_.inactive_timeout()));
	timer_.async_wait(boost::bind(&connection_impl::handle_inactive_timeout, self, 
		asio::placeholders::error));
}

void
connection_impl::handle_error(syst::error_code const &code) {
	logger_->error("error occured with connection[%lu] from %s: %s", id(), address(), code.message().c_str());
	cleanup();
}

void
connection_impl::handle_exception(std::exception const &exc) {
	logger_->error("exception caught with connection[%lu] from %s: %s", id(), address(), exc.what());
	cleanup();
}

void
connection_impl::print_error(std::streambuf &buf, http_error const &http) const {
	
	std::ostream stream(&buf);
	std::string content(http.what());
	
	stream << http_status(http.code());
	stream << http_date(boost::posix_time::second_clock::universal_time());
	stream << http_header::server();
	stream << http_header::connection_close();
	stream << http_header("Content-Type", "text/plain; charset=utf-8");
	stream << http_constants::endl;
}

void
connection_impl::print_headers(char const *content_type, std::streambuf &buf) const {

	std::ostream stream(&buf);
	stream << http_status(200);
	stream << http_date(boost::posix_time::second_clock::universal_time());
	stream << http_header::server();
	stream << http_header::connection_close();
	stream << http_header("Content-Type", content_type);
	stream << http_header("Transfer-Encoding", "chunked");
	stream << http_constants::endl;
}

}} // namespaces
