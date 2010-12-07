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

#ifndef XIVA_DETAILS_SSL_CONNECTION_SOCKET_HPP_INCLUDED
#define XIVA_DETAILS_SSL_CONNECTION_SOCKET_HPP_INCLUDED

#include "xiva/forward.hpp"
#include "details/asio.hpp"

namespace xiva { namespace details {

class ssl_connection_socket {
public:
	ssl_connection_socket(asio::io_service &io, asio::ssl::context& context);
	virtual ~ssl_connection_socket();

	typedef asio::ssl::stream<asio::ip::tcp::socket> ssl_stream_socket;

	ssl_stream_socket::lowest_layer_type& raw_sock();
	ssl_stream_socket& native_sock();

	template <typename HandshakeHandler>
	void async_handshake(HandshakeHandler handler);

	void close();

private:
	ssl_connection_socket(ssl_connection_socket const &);
	ssl_connection_socket& operator = (ssl_connection_socket const &);

	ssl_stream_socket socket_;
};

inline ssl_connection_socket::ssl_stream_socket::lowest_layer_type&
ssl_connection_socket::raw_sock() {
	return socket_.lowest_layer();
}

inline ssl_connection_socket::ssl_stream_socket&
ssl_connection_socket::native_sock() {
	return socket_;
}

template <typename HandshakeHandler> void
ssl_connection_socket::async_handshake(HandshakeHandler handler) {
	socket_.async_handshake(asio::ssl::stream_base::server, handler);
}

inline void
ssl_connection_socket::close() {
	syst::error_code code;
	socket_.shutdown(code);
	socket_.lowest_layer().close();
}
			
}} // namespaces

#endif // XIVA_DETAILS_SSL_CONNECTION_SOCKET_HPP_INCLUDED
