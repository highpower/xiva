/** @file acceptor.hpp */
// xiva (acronym for HTTP Extended EVent Automata) is a simple HTTP server.
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

#ifndef XIVA_DETAILS_ACCEPTOR_HPP_INCLUDED
#define XIVA_DETAILS_ACCEPTOR_HPP_INCLUDED

#include <boost/bind.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/current_function.hpp>

#include "xiva/error.hpp"
#include "xiva/logger.hpp"
#include "xiva/forward.hpp"

#include "details/asio.hpp"
#include "details/acceptor_base.hpp"
#include "details/connection_data.hpp"
#include "details/connection_impl.hpp"

namespace xiva { namespace details {

class connection_data;

template <typename ConnectionBase, typename ConnectionTraits>
class acceptor : public acceptor_base {

public:
	acceptor(asio::io_service &io, connection_data const &data, ConnectionTraits &ct);
	virtual ~acceptor();

	typedef connection_impl<ConnectionBase, ConnectionTraits> ConnectionImpl;
	typedef acceptor<ConnectionBase, ConnectionTraits> Acceptor;

	typedef boost::intrusive_ptr<ConnectionImpl> connection_ptr_type;
	typedef boost::intrusive_ptr<Acceptor> acceptor_ptr_type;

	void accept();
	void accept_again();

	void handle_accept_first(connection_ptr_type conn, syst::error_code const &code);
	void handle_accept_again(connection_ptr_type conn, syst::error_code const &code);
	void process_connection(connection_ptr_type conn);

	void attach_logger(boost::intrusive_ptr<logger> const &log);
	void bind(std::string const &addr, unsigned short port, unsigned short backlog);
	void stop();

private:
	asio::io_service &io_;
	connection_data const &data_;
	ConnectionTraits &ct_;
	asio::ip::tcp::acceptor acceptor_;
	boost::intrusive_ptr<logger> logger_;
};


template <typename ConnectionBase, typename ConnectionTraits>
acceptor<ConnectionBase, ConnectionTraits>::acceptor(asio::io_service &io, connection_data const &data, ConnectionTraits &ct) :
	io_(io), data_(data), ct_(ct), acceptor_(io_)
{
}

template <typename ConnectionBase, typename ConnectionTraits>
acceptor<ConnectionBase, ConnectionTraits>::~acceptor() {
}

template <typename ConnectionBase, typename ConnectionTraits> void
acceptor<ConnectionBase, ConnectionTraits>::accept() {
	try {
		acceptor_ptr_type self(this);
		connection_ptr_type conn(new ConnectionImpl(io_, data_, ct_));
		acceptor_.async_accept(conn->socket(), boost::bind(&acceptor<ConnectionBase, ConnectionTraits>::handle_accept_first,
		                       self, conn, asio::placeholders::error));
	}
	catch (std::exception const &e) {
		logger_->error("exception caught in %s: %s", BOOST_CURRENT_FUNCTION, e.what());
	}
}

template <typename ConnectionBase, typename ConnectionTraits> void
acceptor<ConnectionBase, ConnectionTraits>::accept_again() {
	try {
		acceptor_ptr_type self(this);
		connection_ptr_type conn(new ConnectionImpl(io_, data_, ct_));
		acceptor_.async_accept(conn->socket(), boost::bind(&acceptor<ConnectionBase, ConnectionTraits>::handle_accept_again,
		                       self, conn, asio::placeholders::error));
	}
	catch (std::exception const &e) {
		logger_->error("exception caught in %s: %s", BOOST_CURRENT_FUNCTION, e.what());
	}
}

template <typename ConnectionBase, typename ConnectionTraits> void
acceptor<ConnectionBase, ConnectionTraits>::handle_accept_first(connection_ptr_type conn, syst::error_code const &code) {
	if (code) {
		throw error("network error occured while first accepting connection: %s",
		            code.message().c_str());
	}
	process_connection(conn);
}

template <typename ConnectionBase, typename ConnectionTraits> void
acceptor<ConnectionBase, ConnectionTraits>::handle_accept_again(connection_ptr_type conn, syst::error_code const &code) {
	if (code) {
		logger_->error("network error occured while accepting connection: %s",
		               code.message().c_str());
		accept_again();
	}
	else {
		process_connection(conn);
	}
}

template <typename ConnectionBase, typename ConnectionTraits> void
acceptor<ConnectionBase, ConnectionTraits>::process_connection(connection_ptr_type conn) {
	conn->start();
	logger_->info("connection with id %lu accepted from %s", conn->id(), conn->address());
	accept_again();
}

template <typename ConnectionBase, typename ConnectionTraits> void
acceptor<ConnectionBase, ConnectionTraits>::attach_logger(boost::intrusive_ptr<logger> const &log) {
	assert(log);
	logger_ = log;
}

template <typename ConnectionBase, typename ConnectionTraits> void
acceptor<ConnectionBase, ConnectionTraits>::bind(std::string const &addr, unsigned short port, unsigned short backlog) {

	asio::ip::address address = asio::ip::address::from_string(addr);
	asio::ip::tcp::endpoint endpoint(address, port);

	acceptor_.open(endpoint.protocol());
	acceptor_.set_option(asio::ip::tcp::acceptor::reuse_address(true));

	acceptor_.bind(endpoint);
	acceptor_.listen(backlog);
}

template <typename ConnectionBase, typename ConnectionTraits> void
acceptor<ConnectionBase, ConnectionTraits>::stop() {
	acceptor_.close();
}

}} // namespaces

#endif // XIVA_DETAILS_ACCEPTOR_HPP_INCLUDED
