/** @file connection_manager_base.hpp */
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

#ifndef XIVA_DETAILS_CONNECTION_MANAGER_BASE_HPP_INCLUDED
#define XIVA_DETAILS_CONNECTION_MANAGER_BASE_HPP_INCLUDED

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/intrusive_ptr.hpp>

#include "xiva/shared.hpp"
#include "xiva/forward.hpp"
#include "xiva/globals.hpp"

namespace xiva { namespace details {

class connection_manager_base : public shared {

public:
	connection_manager_base();
	virtual ~connection_manager_base();

	typedef boost::intrusive_ptr<connection_listener> listener_ptr_type;

	virtual void finish() = 0;
	virtual void wait_for_complete() = 0;
	virtual void init(settings const &s) = 0;
	virtual void add_connection_listener(listener_ptr_type const &l) = 0;
	virtual void attach_logger(boost::intrusive_ptr<logger> const &log) = 0;

	virtual void send(std::string const &to, boost::shared_ptr<message> const &m) = 0;
	virtual void send(globals::connection_id const &to, boost::shared_ptr<message> const &m) = 0;

private:
	connection_manager_base(connection_manager_base const &);
	connection_manager_base& operator = (connection_manager_base const &);
};

}} // namespaces

#endif // XIVA_DETAILS_CONNECTION_MANAGER_BASE_HPP_INCLUDED
