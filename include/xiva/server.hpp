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

#ifndef XIVA_SERVER_HPP_INCLUDED
#define XIVA_SERVER_HPP_INCLUDED

#include <string>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/intrusive_ptr.hpp>

#include "xiva/config.hpp"
#include "xiva/forward.hpp"
#include "xiva/globals.hpp"

namespace xiva {

class XIVA_API server {

public:
	server();
	virtual ~server();

	void stop();
	void start(settings const &s);

	void send(std::string const &to, boost::shared_ptr<message> const &m);
	void send(globals::connection_id to, boost::shared_ptr<message> const &m);

	component_set& components();
	void start_provider(unsigned short nthreads, boost::function<globals::provider_type> f);

	void attach_logger(boost::intrusive_ptr<logger> const &log);
	void attach_response_handler(boost::intrusive_ptr<response_handler> const &m);
	void attach_formatter_creator(std::string const &fmt_id, boost::intrusive_ptr<formatter_creator> const &creator);
	void add_connection_listener(boost::intrusive_ptr<connection_listener> const &l);
	void attach_message_filter(boost::intrusive_ptr<message_filter> const &filter);

private:
	boost::shared_ptr<details::server_impl> impl_;
};

} // namespace

#endif // XIVA_SERVER_HPP_INCLUDED
