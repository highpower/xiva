// Copyright (C) 2009 Yandex <highpower@yandex.ru>

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef XIVA_DETAILS_CONNECTION_IMPL_HPP_INCLUDED
#define XIVA_DETAILS_CONNECTION_IMPL_HPP_INCLUDED

#include <list>
#include <iosfwd>
#include <string>
#include <cassert>

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

#include "xiva/logger.hpp"
#include "xiva/forward.hpp"
#include "xiva/message.hpp"
#include "xiva/http_error.hpp"
#include "xiva/formatter.hpp"

#include "details/asio.hpp"
#include "details/connection_data.hpp"
#include "details/request_impl.hpp"
#include "details/response_impl.hpp"
#include "details/request_checker.hpp"

namespace xiva { namespace details {

template <typename ConnectionBase, typename ConnectionTraits>
class connection_impl : public ConnectionBase {

public:
	connection_impl(asio::io_service &io, connection_data const &data, ConnectionTraits &ct);
	virtual ~connection_impl();

	typedef connection_impl<ConnectionBase, ConnectionTraits> type;
	typedef typename boost::intrusive_ptr<type> connection_impl_ptr_type;

	typedef boost::shared_ptr<message> message_ptr_type;

	virtual void start();
	virtual void finish();
	virtual void send(message_ptr_type const &message);

	void handle_read(syst::error_code const &code);
	void handle_write_headers(syst::error_code const &code);
	void handle_write_message(syst::error_code const &code);
	void handle_write_ping_message(syst::error_code const &code);
	void handle_ping(syst::error_code const &code);
	void handle_cleanup(syst::error_code const &code);
	void handle_timeout(syst::error_code const &code);
	void handle_inactive_timeout(syst::error_code const &code);

	char const* address() const;
	asio::ip::tcp::socket& socket();
	virtual void handled(request_impl const &req, response_impl const &resp);

private:
	typedef std::allocator<char> allocator_type;
	typedef asio::basic_streambuf<allocator_type> streambuf_type;

	void read();
	void write_static_content(response_impl const &resp, std::string const &content);
	void write_headers(response_impl const &resp);
	void write_message();
	void write_ping_message();
	void write_last_message();
	void write_policy_data();
	void write_http_error(http_error const &http, char const *error_msg = NULL);

	void cleanup();
	void setup_timeout(unsigned int timeout);
	void setup_ping_or_inactive_timeout();
	void setup_inactive_timeout();
	void handle_error(syst::error_code const &code);
	void handle_exception(std::exception const &exc);

private:
	asio::io_service &io_;
	connection_data const &data_;
	ConnectionTraits &ct_;

	std::string addr_;
	streambuf_type in_, out_;

	asio::deadline_timer timer_;
	boost::posix_time::ptime inactive_expires_;
	asio::ip::tcp::socket socket_;
	std::list<message_ptr_type> messages_;

	bool writing_message_;
	bool connected_;
	bool is_policy_;
	bool single_message_;
	bool disable_ping_;
	bool managed_;

	static const unsigned int MINIMAL_PING_INTERVAL = 100;
	static const unsigned int MINIMAL_INACTIVE_TIMEOUT = 100;
};

template <typename ConnectionBase, typename ConnectionTraits>
connection_impl<ConnectionBase, ConnectionTraits>::connection_impl(asio::io_service &io, connection_data const &data, ConnectionTraits &ct) :
	io_(io), data_(data), ct_(ct), timer_(io_), socket_(io_),
	writing_message_(false), connected_(false), is_policy_(false), single_message_(false), disable_ping_(false), managed_(false)
{
}

template <typename ConnectionBase, typename ConnectionTraits>
connection_impl<ConnectionBase, ConnectionTraits>::~connection_impl() {

}

template <typename ConnectionBase, typename ConnectionTraits> inline char const*
connection_impl<ConnectionBase, ConnectionTraits>::address() const {
	return addr_.c_str();
}

template <typename ConnectionBase, typename ConnectionTraits> inline asio::ip::tcp::socket&
connection_impl<ConnectionBase, ConnectionTraits>::socket() {
	return socket_;
}


template <typename ConnectionBase, typename ConnectionTraits> void
connection_impl<ConnectionBase, ConnectionTraits>::start() {
	try {
    		asio::ip::tcp::endpoint endpoint = socket_.remote_endpoint();
		asio::ip::address addr = endpoint.address();
		addr_ = addr.to_string();
		read();
	} 
	catch (std::exception const &e) {
		handle_exception(e);
	}
}

template <typename ConnectionBase, typename ConnectionTraits> void
connection_impl<ConnectionBase, ConnectionTraits>::finish() {
	send(boost::shared_ptr<message>());
}

template <typename ConnectionBase, typename ConnectionTraits> void
connection_impl<ConnectionBase, ConnectionTraits>::send(boost::shared_ptr<message> const &m) {

	if (m) {
		data_.log()->debug("sending message to connection[%lu], name %s", ConnectionBase::id(), ConnectionBase::name().c_str());
	}
	messages_.push_back(m);
	if (!writing_message_ && connected_) {
		write_message();
	}
}

template <typename ConnectionBase, typename ConnectionTraits> void
connection_impl<ConnectionBase, ConnectionTraits>::handled(request_impl const &req, response_impl const &resp) {
	timer_.cancel();
	try {
		std::string const *content = resp.content_ptr();
		if (NULL != content) {
			write_static_content(resp, *content);
			return;
		}
		std::string const &name = ConnectionBase::name();
		if (name.empty()) {
			throw http_error(http_error::not_found);
		}
		data_.log()->debug("name %s assigned to connection[%lu] from %s", name.c_str(), ConnectionBase::id(), address());
		single_message_ = resp.single_message();
		ConnectionBase::init_formatters(data_.fmt_factory(), req, resp);
		disable_ping_ =	single_message_ || data_.ping_interval() < MINIMAL_PING_INTERVAL ||
				NULL == ConnectionBase::default_formatter();
		write_headers(resp);
		boost::intrusive_ptr<ConnectionBase> self(this);
		ct_.manager().insert_connection(self);
		managed_ = true;
	}
	catch (http_error const &h) {
		write_http_error(h);
	}
	catch (std::exception const &e) {
		handle_exception(e);
	}
}

template <typename ConnectionBase, typename ConnectionTraits> void
connection_impl<ConnectionBase, ConnectionTraits>::handle_read(syst::error_code const &code) {
	timer_.cancel();
	if (code) {
		handle_error(code);
		return;
	}

	bool parsing_request = false;
	try {
		typedef streambuf_type::const_buffers_type buffers_type;
		typedef asio::buffers_iterator<buffers_type> iterator_type;

		buffers_type data = in_.data();
		iterator_type begin = iterator_type::begin(data);
		iterator_type end = iterator_type::end(data);
		is_policy_ = data_.is_policy(begin, end);
		if (is_policy_) {
			write_policy_data();
		}
		else {
			parsing_request = true;
			request_impl req(begin, end);
			ConnectionBase::init(req);
			parsing_request = false;
			
			response_impl resp;
			boost::intrusive_ptr<ConnectionBase> self(this);
			ct_.handler_invoker().invoke_handler(self, req, resp);
		}
	}
	catch (http_error const &h) {
		write_http_error(h);
	}
	catch (std::exception const &e) {
		if (parsing_request) {
			http_error err(http_error::bad_request);
			write_http_error(err, e.what());
		}
		else {
			handle_exception(e);
		}
	}
}

template <typename ConnectionBase, typename ConnectionTraits> void
connection_impl<ConnectionBase, ConnectionTraits>::handle_write_headers(syst::error_code const &code) {
	timer_.cancel();
	if (code) {
		handle_error(code);
		return;
	}
	if (socket_.is_open()) {
		connected_ = true;
		write_message();
	}
	else {
		// unreachable code, not connected after cleanup();
	}
}

template <typename ConnectionBase, typename ConnectionTraits> void
connection_impl<ConnectionBase, ConnectionTraits>::handle_write_message(syst::error_code const &code) {
	timer_.cancel();
	writing_message_ = false;
	if (code) {
		handle_error(code);
		return;
	}
	if (single_message_) {
		write_last_message();
	}
	else {
		write_message();
	}
}

template <typename ConnectionBase, typename ConnectionTraits> void
connection_impl<ConnectionBase, ConnectionTraits>::handle_write_ping_message(syst::error_code const &code) {
	timer_.cancel();
	writing_message_ = false;
	if (code) {
		handle_error(code);
		return;
	}
	assert(!disable_ping_);
	if (!messages_.empty()) {
		write_message();
	}
	else {
		setup_ping_or_inactive_timeout();
	}
}

template <typename ConnectionBase, typename ConnectionTraits> void
connection_impl<ConnectionBase, ConnectionTraits>::handle_ping(syst::error_code const &code) {
	assert(!disable_ping_);
	if (!code) {
		write_ping_message();
	}
}

template <typename ConnectionBase, typename ConnectionTraits> void
connection_impl<ConnectionBase, ConnectionTraits>::handle_cleanup(syst::error_code const &code) {
	(void) code;
	cleanup();
}

template <typename ConnectionBase, typename ConnectionTraits> void
connection_impl<ConnectionBase, ConnectionTraits>::handle_timeout(syst::error_code const &code) {
	if (!code) {
		data_.log()->info("timeout was reached with connection[%lu] from %s", ConnectionBase::id(), address());
		cleanup();
	}
}

template <typename ConnectionBase, typename ConnectionTraits> void
connection_impl<ConnectionBase, ConnectionTraits>::handle_inactive_timeout(syst::error_code const &code) {
	if (!code) {
		data_.log()->info("inactive timeout was reached with connection[%lu] from %s", ConnectionBase::id(), address());
		write_last_message();
	}
}

template <typename ConnectionBase, typename ConnectionTraits> void
connection_impl<ConnectionBase, ConnectionTraits>::read() {

	try {
		connection_impl_ptr_type self(this);
		asio::async_read_until(socket_, in_, request_checker(), 
			boost::bind(&type::handle_read, self, asio::placeholders::error));
		setup_timeout(data_.read_timeout());
	}
	catch (std::exception const &e) {
		handle_exception(e);
	}
}

template <typename ConnectionBase, typename ConnectionTraits> void
connection_impl<ConnectionBase, ConnectionTraits>::write_static_content(response_impl const &resp, std::string const &content) {

	try {
		if (is_policy_ || !ConnectionBase::print_static_content(resp.content_type(), content, out_)) {
			cleanup();
			return;
		}

		writing_message_ = true;
		connection_impl_ptr_type self(this);
		asio::async_write(socket_, out_, boost::bind(&type::handle_cleanup,
			self, asio::placeholders::error));
		setup_timeout(data_.write_timeout()); // static_content_write_timeout ?
	}
	catch (std::exception const &e) {
		handle_exception(e);
	}
}

template <typename ConnectionBase, typename ConnectionTraits> void
connection_impl<ConnectionBase, ConnectionTraits>::write_headers(response_impl const &resp) {

	if (is_policy_ || !ConnectionBase::print_headers(resp.content_type(), out_)) {
		cleanup();
		return;
	}

	connection_impl_ptr_type self(this);
	asio::async_write(socket_, out_, boost::bind(&type::handle_write_headers, self,
		asio::placeholders::error));
	setup_timeout(data_.write_timeout());
}

template <typename ConnectionBase, typename ConnectionTraits> void
connection_impl<ConnectionBase, ConnectionTraits>::write_message() {

	try {
		while (connected_) {
			timer_.cancel();
			if (messages_.empty()) {
				setup_inactive_timeout();
				return;
			}
			boost::shared_ptr<message> const &msg = messages_.front();
			if (!msg) {
				write_last_message();
				return;
			}

			std::string const &content = msg->content();
			bool printed = false;
			formatter const *fmt_ptr = ConnectionBase::find_formatter(*msg);
			if (NULL != fmt_ptr) {
				printed = ConnectionBase::print_message_content(fmt_ptr->wrap_message(content), out_);
			}
			else {
				printed = ConnectionBase::print_message_content(content, out_);
			}
			if (printed) {
				ConnectionBase::notify_message_printed(*msg);
			}
			messages_.pop_front();

			if (printed) {
				writing_message_ = true;
				connection_impl_ptr_type self(this);
				asio::async_write(socket_, out_, boost::bind(&type::handle_write_message,
					self, asio::placeholders::error));
				setup_timeout(data_.write_timeout());
				return;
			}
			// skip empty message
		}
	}
	catch (std::exception const &e) {
		handle_exception(e);
	}
}

template <typename ConnectionBase, typename ConnectionTraits> void
connection_impl<ConnectionBase, ConnectionTraits>::write_ping_message() {

	try {
		assert(!disable_ping_);
		formatter const *fmt_ptr = ConnectionBase::default_formatter();
		assert(fmt_ptr);
		if (ConnectionBase::print_message_content(fmt_ptr->ping_message(), out_)) {
			writing_message_ = true;
			connection_impl_ptr_type self(this);
			asio::async_write(socket_, out_, boost::bind(&type::handle_write_ping_message,
				self, asio::placeholders::error));
			setup_timeout(data_.write_timeout());
		}
		else {
			disable_ping_ = true;
			setup_ping_or_inactive_timeout();
		}
	}
	catch (std::exception const &e) {
		handle_exception(e);
	}
}

template <typename ConnectionBase, typename ConnectionTraits> void
connection_impl<ConnectionBase, ConnectionTraits>::write_last_message() {

	try {
		if (is_policy_ || !ConnectionBase::print_last_message(out_)) {
			cleanup();
			return;
		}

		writing_message_ = true;
		connection_impl_ptr_type self(this);
		asio::async_write(socket_, out_, boost::bind(&type::handle_cleanup,
			self, asio::placeholders::error));
		setup_timeout(data_.write_timeout());
	}
	catch (std::exception const &e) {
		handle_exception(e);
	}
}

template <typename ConnectionBase, typename ConnectionTraits> void
connection_impl<ConnectionBase, ConnectionTraits>::write_policy_data() {

	try {
		if (!ConnectionBase::print_policy_data(data_.policy_data(), out_)) {
			data_.log()->error("can not answer policy data for connection[%lu] from %s", ConnectionBase::id(), address());
			cleanup();
			return;
		}

		data_.log()->debug("policy request for connection[%lu] from %s", ConnectionBase::id(), address());

		writing_message_ = true;
		connection_impl_ptr_type self(this);
		asio::async_write(socket_, out_, boost::bind(&type::handle_cleanup,
			self, asio::placeholders::error));
		setup_timeout(data_.write_timeout());
	}
	catch (std::exception const &e) {
		handle_exception(e);
	}
}

template <typename ConnectionBase, typename ConnectionTraits> void
connection_impl<ConnectionBase, ConnectionTraits>::write_http_error(http_error const &http, char const *error_msg) {

	data_.log()->error("http error %u occured with connection[%lu] from %s: %s",
		(unsigned int)http.code(), ConnectionBase::id(), address(), error_msg ? error_msg : http.what());
	try {
		if (is_policy_ || !ConnectionBase::print_error(out_, http)) {
			cleanup();
			return;
		}

		connection_impl_ptr_type self(this);
		asio::async_write(socket_, out_, boost::bind(&type::handle_cleanup,
			self, asio::placeholders::error));
		setup_timeout(data_.write_timeout());
	}
	catch (std::exception const &e) {
		handle_exception(e);
	}
}

template <typename ConnectionBase, typename ConnectionTraits> void
connection_impl<ConnectionBase, ConnectionTraits>::cleanup() {
	timer_.cancel();
	connected_ = false;
	if (managed_ && !ConnectionBase::name().empty()) {
		managed_ = false;
		boost::intrusive_ptr<ConnectionBase> self(this);
		ct_.manager().remove_connection(self);
	}
	if (socket_.is_open()) {
		socket_.close();
		data_.log()->info("connection[%lu] from %s is closed", ConnectionBase::id(), address());
	}
}

template <typename ConnectionBase, typename ConnectionTraits> void
connection_impl<ConnectionBase, ConnectionTraits>::setup_timeout(unsigned int timeout) {

	connection_impl_ptr_type self(this);
	timer_.expires_from_now(boost::posix_time::milliseconds(timeout));
	timer_.async_wait(boost::bind(&type::handle_timeout, self,
		asio::placeholders::error));
}

template <typename ConnectionBase, typename ConnectionTraits> void
connection_impl<ConnectionBase, ConnectionTraits>::setup_ping_or_inactive_timeout() {

	boost::posix_time::ptime now = boost::posix_time::microsec_clock::universal_time();
	if (inactive_expires_ <= now) {
		boost::posix_time::time_duration td = boost::posix_time::milliseconds(data_.inactive_timeout());
		inactive_expires_ = now + td;
	}

	if (!disable_ping_) {
		boost::posix_time::time_duration td = boost::posix_time::milliseconds(data_.ping_interval());
		boost::posix_time::ptime ping_time = now + td;
		if (ping_time < inactive_expires_) {
			timer_.expires_from_now(td);
			connection_impl_ptr_type self(this);
			timer_.async_wait(boost::bind(&type::handle_ping, self,
				asio::placeholders::error));
			return;
		}
	}

	if (inactive_expires_ - now >= boost::posix_time::milliseconds(MINIMAL_INACTIVE_TIMEOUT)) {
		connection_impl_ptr_type self(this);
		timer_.expires_at(inactive_expires_);
		timer_.async_wait(boost::bind(&type::handle_inactive_timeout, self,
			asio::placeholders::error));
	}
	else {
		handle_inactive_timeout(syst::error_code());
	}
}

template <typename ConnectionBase, typename ConnectionTraits> void
connection_impl<ConnectionBase, ConnectionTraits>::setup_inactive_timeout() {

	boost::posix_time::time_duration td = boost::posix_time::milliseconds(data_.inactive_timeout());
	inactive_expires_ = boost::posix_time::microsec_clock::universal_time() + td;

	setup_ping_or_inactive_timeout();
}

template <typename ConnectionBase, typename ConnectionTraits> void
connection_impl<ConnectionBase, ConnectionTraits>::handle_error(syst::error_code const &code) {
	data_.log()->error("error occured with connection[%lu] from %s: %s", ConnectionBase::id(), address(), code.message().c_str());
	cleanup();
}

template <typename ConnectionBase, typename ConnectionTraits> void
connection_impl<ConnectionBase, ConnectionTraits>::handle_exception(std::exception const &exc) {
	if (!addr_.empty()) {
		data_.log()->error("exception caught with connection[%lu] from %s: %s", ConnectionBase::id(), address(), exc.what());
	}
	cleanup();
}

}} // namespaces

#endif // XIVA_DETAILS_CONNECTION_IMPL_HPP_INCLUDED
