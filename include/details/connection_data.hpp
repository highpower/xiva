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

#ifndef XIVA_DETAILS_CONNECTION_DATA_HPP_INCLUDED
#define XIVA_DETAILS_CONNECTION_DATA_HPP_INCLUDED

#include <string>
#include <memory>
#include <boost/intrusive_ptr.hpp>

#include "xiva/forward.hpp"
#include "xiva/globals.hpp"

namespace xiva { namespace details {

class connection_data {

public:
	explicit connection_data(server_impl &server);
	virtual ~connection_data();

	void init(settings const &s, boost::intrusive_ptr<ping_formatter> const &f);

	unsigned int read_timeout() const;
	unsigned int write_timeout() const;
	unsigned int inactive_timeout() const;
	unsigned int ping_interval() const;
	unsigned int max_request_size() const;

	boost::intrusive_ptr<response_handler> const& handler() const;
	void handler(boost::intrusive_ptr<response_handler> const &h);

	boost::intrusive_ptr<logger> const& log() const;
	void attach_logger(boost::intrusive_ptr<logger> const &log);

	formatters_factory const& fmt_factory() const;
	formatters_factory& fmt_factory();

	std::string const& ping_message(bool single) const;
	std::string const& policy_data() const;

	bool stopping() const;
	void stop();

	void notify_connection_opened_failed(std::string const &to, globals::connection_id id) const;

private:
	connection_data(connection_data const &);
	connection_data & operator = (connection_data const &);

	server_impl &server_;
	std::auto_ptr<formatters_factory> formatters_factory_;
	std::string ping_message_;
	std::string ping_message_for_single_;
	std::string policy_data_;
	boost::intrusive_ptr<logger> logger_;
	boost::intrusive_ptr<response_handler> handler_;
	unsigned int read_timeout_, write_timeout_, inactive_timeout_;
	unsigned int ping_interval_;
	unsigned int max_request_size_;
	bool stopping_;
};

inline unsigned int
connection_data::read_timeout() const {
	return read_timeout_;
}

inline unsigned int
connection_data::write_timeout() const {
	return write_timeout_;
}

inline unsigned int
connection_data::inactive_timeout() const {
	return inactive_timeout_;
}

inline unsigned int
connection_data::ping_interval() const {
	return ping_interval_;
}

inline unsigned int
connection_data::max_request_size() const {
	return max_request_size_;
}

inline std::string const&
connection_data::ping_message(bool single) const {
	return single ? ping_message_for_single_ : ping_message_;
}

inline std::string const&
connection_data::policy_data() const {
	return policy_data_;
}

inline boost::intrusive_ptr<logger> const&
connection_data::log() const {
	return logger_;
}

}} // namespaces

#endif // XIVA_DETAILS_CONNECTION_DATA_HPP_INCLUDED
