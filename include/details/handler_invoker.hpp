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

#ifndef XIVA_DETAILS_HANDLER_INVOKER_HPP_INCLUDED
#define XIVA_DETAILS_HANDLER_INVOKER_HPP_INCLUDED

#include <string>
#include <exception>
#include <boost/intrusive_ptr.hpp>

#include "xiva/shared.hpp"
#include "xiva/logger.hpp"
#include "xiva/forward.hpp"

namespace xiva { namespace details {

class connection;
class request_impl;
class connection_data;

class handler_invoker : public shared {

public:
	explicit handler_invoker(connection_data const &data);
	virtual ~handler_invoker();

	typedef connection connection_type;
	typedef boost::intrusive_ptr<connection_type> connection_ptr_type;

	void init(settings const &s);
	void attach_logger(boost::intrusive_ptr<logger> const &log);
	void invoke_handler(connection_ptr_type conn, request_impl &req, response_impl &resp);

private:
	handler_invoker(handler_invoker const &);
	handler_invoker& operator = (handler_invoker const &);

private:
	boost::intrusive_ptr<response_handler> handler_;
};

}} // namespaces

#endif // XIVA_DETAILS_HANDLER_INVOKER_HPP_INCLUDED
