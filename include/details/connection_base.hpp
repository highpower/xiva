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

#ifndef XIVA_DETAILS_CONNECTION_BASE_HPP_INCLUDED
#define XIVA_DETAILS_CONNECTION_BASE_HPP_INCLUDED

#include <iosfwd>
#include <string>
#include <memory>

#include <boost/shared_ptr.hpp>

#include "xiva/forward.hpp"
#include "xiva/globals.hpp"

#include "details/guard.hpp"
#include "details/websocket_info.hpp"

namespace xiva { namespace details {

class formatters_data;

class connection_base {

public:
	connection_base();
	virtual ~connection_base();

	virtual void close() = 0;
	virtual void finish() = 0;
	virtual void send(boost::shared_ptr<message> const &message) = 0;
	virtual void handled(request_impl const &req, response_impl const &resp) = 0;
	virtual void handled_errors(
		request_impl const &req, response_impl const &resp,
		unsigned short http_code, std::string const &error_msg) = 0;

	globals::connection_id id() const;

	std::string const& name() const;
	void name(std::string const &name);

	bool allow_message(message const &msg, message_filter const *filter) const;
	void update_channels_stat(channels_stat_impl &ch_stat, bool add) const;

	boost::shared_ptr<guard> const& get_guard() const;

protected:
	void init(request_impl const &req, bool secure);
	void init_formatters(formatters_factory const &f, request_impl const &req, response_impl const &resp);

	formatter const* find_formatter(message const &msg) const;

	void notify_message_printed(message const &msg);

	static bool print_policy_data(std::string const &data, std::streambuf &buf);

	bool print_error(std::streambuf &buf, http_error const &error) const;
	bool print_headers(std::string const &content_type, std::streambuf &buf) const;
	bool print_static_content(std::string const &content_type, std::string const &content, std::streambuf &buf) const;
	bool print_message_content(std::string const &content, std::streambuf &buf) const;
	bool print_last_message(std::streambuf &buf) const;

private:
	connection_base(connection_base const &);
	connection_base& operator = (connection_base const &);

	std::auto_ptr<formatters_data> fmt_data_;
	websocket_info ws_info_;
	std::string name_;
	boost::shared_ptr<guard> guard_;
};

inline std::string const&
connection_base::name() const {
	return name_;
}
	
inline void
connection_base::name(std::string const &name) {
	name_.assign(name);
}

inline boost::shared_ptr<guard> const&
connection_base::get_guard() const {
	return guard_;
}

			
}} // namespaces

#endif // XIVA_DETAILS_CONNECTION_BASE_HPP_INCLUDED
