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

#ifndef XIVA_DETAILS_CONNECTION_TRAITS_HPP_INCLUDED
#define XIVA_DETAILS_CONNECTION_TRAITS_HPP_INCLUDED

#include <cassert>
#include <boost/intrusive_ptr.hpp>

#include "xiva/forward.hpp"
#include "xiva/logger.hpp"

#include "details/asio.hpp"
#include "details/connection_manager.hpp"
#include "details/connection_traits_base.hpp"
#include "details/connection_socket.hpp"

namespace xiva { namespace details {

template <typename HandlerInvoker>
class connection_traits : public connection_traits_base {

public:
	typedef connection_socket socket_type;
	typedef typename HandlerInvoker::connection_type connection_type;
	typedef connection_manager<connection_type> connection_manager_type;
	typedef boost::intrusive_ptr<HandlerInvoker> handler_invoker_ptr_type;
	typedef boost::intrusive_ptr<connection_manager_type> connection_manager_ptr_type;
	
	connection_traits(connection_manager_ptr_type cm, handler_invoker_ptr_type cv);
	virtual ~connection_traits();

	static bool secure();
	static std::auto_ptr<connection_socket> create_socket(asio::io_service &io);

	HandlerInvoker& handler_invoker();
	connection_manager_type& manager();
	
private:
	connection_traits(connection_traits const &);
	connection_traits& operator = (connection_traits const &);

private:
	handler_invoker_ptr_type hi_;
	connection_manager_ptr_type cm_;
};

template <typename HandlerInvoker> inline
connection_traits<HandlerInvoker>::connection_traits(connection_manager_ptr_type  cm, handler_invoker_ptr_type hi) :
	hi_(hi), cm_(cm)
{
	assert(hi_);
	assert(cm_);
}

template <typename HandlerInvoker> inline
connection_traits<HandlerInvoker>::~connection_traits() {
}

template <typename HandlerInvoker> inline bool
connection_traits<HandlerInvoker>::secure() {
	return false;
}

template <typename HandlerInvoker> std::auto_ptr<connection_socket>
connection_traits<HandlerInvoker>::create_socket(asio::io_service &io) {
	return std::auto_ptr<socket_type>(new connection_socket(io));
}

template <typename HandlerInvoker> inline HandlerInvoker&
connection_traits<HandlerInvoker>::handler_invoker() {
	return *hi_;
}

template <typename HandlerInvoker> inline connection_manager< typename HandlerInvoker::connection_type >&
connection_traits<HandlerInvoker>::manager() {
	return *cm_;
}

}} // namespaces

#endif // XIVA_DETAILS_CONNECTION_TRAITS_HPP_INCLUDED
