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

#ifndef XIVA_DETAILS_SSL_CONNECTION_TRAITS_HPP_INCLUDED
#define XIVA_DETAILS_SSL_CONNECTION_TRAITS_HPP_INCLUDED

#include <memory>
#include <boost/intrusive_ptr.hpp>

#include "xiva/forward.hpp"
#include "details/asio.hpp"
#include "details/connection_traits.hpp"
#include "details/ssl_context.hpp"
#include "details/ssl_connection_socket.hpp"

namespace xiva { namespace details {

template <typename HandlerInvoker>
class ssl_connection_traits : public connection_traits<HandlerInvoker> {

public:
	typedef ssl_connection_socket socket_type;
	typedef typename HandlerInvoker::connection_type connection_type;
	typedef connection_manager<connection_type> connection_manager_type;
	typedef boost::intrusive_ptr<HandlerInvoker> handler_invoker_ptr_type;
	typedef boost::intrusive_ptr<connection_manager_type> connection_manager_ptr_type;

	ssl_connection_traits(asio::io_service &io, connection_manager_ptr_type cm, handler_invoker_ptr_type hi);
	virtual ~ssl_connection_traits();

	static bool secure();
	std::auto_ptr<ssl_connection_socket> create_socket(asio::io_service &io);

	void init(settings const &s);

private:
	ssl_connection_traits(ssl_connection_traits const &);
	ssl_connection_traits& operator = (ssl_connection_traits const &);

private:
	ssl_context context_;
};

template <typename HandlerInvoker> inline
ssl_connection_traits<HandlerInvoker>::ssl_connection_traits(asio::io_service &io, connection_manager_ptr_type cm, handler_invoker_ptr_type hi) :
	connection_traits<HandlerInvoker>(cm, hi), context_(io)
{
}

template <typename HandlerInvoker> inline
ssl_connection_traits<HandlerInvoker>::~ssl_connection_traits() {
}

template <typename HandlerInvoker> inline bool
ssl_connection_traits<HandlerInvoker>::secure() {
	return true;
}

template <typename HandlerInvoker> std::auto_ptr<ssl_connection_socket>
ssl_connection_traits<HandlerInvoker>::create_socket(asio::io_service &io) {
	return context_.create_socket(io);
}

template <typename HandlerInvoker> void
ssl_connection_traits<HandlerInvoker>::init(settings const &s) {
	context_.init(s);
}

}} // namespaces

#endif // XIVA_DETAILS_SSL_CONNECTION_TRAITS_HPP_INCLUDED
