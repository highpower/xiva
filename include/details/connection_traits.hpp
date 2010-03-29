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

#include "details/connection_traits_base.hpp"
#include "details/connection_manager.hpp"

namespace xiva { namespace details {

template <typename ConnectionBase, typename MatcherInvoker>
class connection_traits : public connection_traits_base {

public:
	typedef connection_manager<ConnectionBase> ConnectionManager;
	typedef boost::intrusive_ptr<ConnectionManager> connection_manager_ptr_type;
	typedef boost::intrusive_ptr<MatcherInvoker> matcher_invoker_ptr_type;

	connection_traits(connection_manager_ptr_type cm, matcher_invoker_ptr_type cv);
	virtual ~connection_traits();

	ConnectionManager& manager();
	MatcherInvoker& matcher_invoker();

	virtual void attach_logger(boost::intrusive_ptr<logger> const &log);
	virtual void init(settings const &s);

private:
	connection_manager_ptr_type cm_;
	matcher_invoker_ptr_type mi_;
};

template <typename ConnectionBase, typename MatcherInvoker> inline
connection_traits<ConnectionBase, MatcherInvoker>::connection_traits(connection_manager_ptr_type cm, matcher_invoker_ptr_type mi) :
	cm_(cm), mi_(mi)
{
	assert(cm);
	assert(mi);
}

template <typename ConnectionBase, typename MatcherInvoker> inline
connection_traits<ConnectionBase, MatcherInvoker>::~connection_traits() {
}


template <typename ConnectionBase, typename MatcherInvoker> inline connection_manager<ConnectionBase>&
connection_traits<ConnectionBase, MatcherInvoker>::manager() {
	return *cm_;
}

template <typename ConnectionBase, typename MatcherInvoker> inline MatcherInvoker&
connection_traits<ConnectionBase, MatcherInvoker>::matcher_invoker() {
	return *mi_;
}

template <typename ConnectionBase, typename MatcherInvoker> inline void
connection_traits<ConnectionBase, MatcherInvoker>::attach_logger(boost::intrusive_ptr<logger> const &log) {
	cm_->attach_logger(log);
	mi_->attach_logger(log);
}

template <typename ConnectionBase, typename MatcherInvoker> inline void
connection_traits<ConnectionBase, MatcherInvoker>::init(settings const &s) {
	cm_->init(s);
	mi_->init(s);
}

}} // namespaces

#endif // XIVA_DETAILS_CONNECTION_TRAITS_HPP_INCLUDED
