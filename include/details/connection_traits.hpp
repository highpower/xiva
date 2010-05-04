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

#include "details/connection_manager.hpp"
#include "details/connection_traits_base.hpp"

namespace xiva { namespace details {

template <typename ConnectionBase, typename HandlerInvoker>
class connection_traits : public connection_traits_base {

public:
	typedef connection_manager<ConnectionBase> connection_manager_type;
	typedef boost::intrusive_ptr<HandlerInvoker> handler_invoker_ptr_type;
	typedef boost::intrusive_ptr<connection_manager_type> connection_manager_ptr_type;
	
	connection_traits(connection_manager_ptr_type cm, handler_invoker_ptr_type cv);
	virtual ~connection_traits();

	HandlerInvoker& handler_invoker();
	connection_manager_type& manager();

	virtual void finish();	
	virtual void init(settings const &s);
	virtual void attach_logger(boost::intrusive_ptr<logger> const &log);
	
private:
	connection_traits(connection_traits const &);
	connection_traits& operator = (connection_traits const &);

private:
	handler_invoker_ptr_type hi_;
	connection_manager_ptr_type cm_;
};

template <typename ConnectionBase, typename HandlerInvoker> inline
connection_traits<ConnectionBase, HandlerInvoker>::connection_traits(typename connection_traits<ConnectionBase, HandlerInvoker>::connection_manager_ptr_type cm, connection_traits<ConnectionBase, HandlerInvoker>::handler_invoker_ptr_type hi) :
	hi_(hi), cm_(cm)
{
	assert(hi_);
	assert(cm_);
}

template <typename ConnectionBase, typename HandlerInvoker> inline
connection_traits<ConnectionBase, HandlerInvoker>::~connection_traits() {
}

template <typename ConnectionBase, typename HandlerInvoker> inline HandlerInvoker&
connection_traits<ConnectionBase, HandlerInvoker>::handler_invoker() {
	return *hi_;
}

template <typename ConnectionBase, typename HandlerInvoker> inline connection_manager<ConnectionBase>&
connection_traits<ConnectionBase, HandlerInvoker>::manager() {
	return *cm_;
}

template <typename ConnectionBase, typename HandlerInvoker> inline void
connection_traits<ConnectionBase, HandlerInvoker>::finish() {
	hi_->finish();
	cm_->finish();
}

template <typename ConnectionBase, typename HandlerInvoker> inline void
connection_traits<ConnectionBase, HandlerInvoker>::init(settings const &s) {
	hi_->init(s);
	cm_->init(s);
}

template <typename ConnectionBase, typename HandlerInvoker> inline void
connection_traits<ConnectionBase, HandlerInvoker>::attach_logger(boost::intrusive_ptr<logger> const &log) {
	hi_->attach_logger(log);
	cm_->attach_logger(log);
}

}} // namespaces

#endif // XIVA_DETAILS_CONNECTION_TRAITS_HPP_INCLUDED
