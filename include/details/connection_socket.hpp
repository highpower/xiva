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

#ifndef XIVA_DETAILS_CONNECTION_SOCKET_HPP_INCLUDED
#define XIVA_DETAILS_CONNECTION_SOCKET_HPP_INCLUDED

#include "xiva/forward.hpp"
#include "details/asio.hpp"

namespace xiva { namespace details {

class connection_socket {
public:
	explicit connection_socket(asio::io_service &io);
	virtual ~connection_socket();

	asio::ip::tcp::socket& raw_sock();
	asio::ip::tcp::socket& native_sock();

	template <typename HandshakeHandler>
	void async_handshake(HandshakeHandler handler);

	void close();

private:
	connection_socket(connection_socket const &);
	connection_socket& operator = (connection_socket const &);

	asio::ip::tcp::socket socket_;
};

inline asio::ip::tcp::socket&
connection_socket::raw_sock() {
	return socket_;
}

inline asio::ip::tcp::socket&
connection_socket::native_sock() {
	return socket_;
}

template <typename HandshakeHandler> inline void
connection_socket::async_handshake(HandshakeHandler /* handler */ ) {
}

inline void
connection_socket::close() {
	syst::error_code code;
	socket_.shutdown(asio::ip::tcp::socket::shutdown_both, code);
	socket_.close();
}

}} // namespaces

#endif // XIVA_DETAILS_CONNECTION_SOCKET_HPP_INCLUDED
