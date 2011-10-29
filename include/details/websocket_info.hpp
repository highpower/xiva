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

#ifndef XIVA_DETAILS_WEBSOCKET_INFO_HPP_INCLUDED
#define XIVA_DETAILS_WEBSOCKET_INFO_HPP_INCLUDED

#include <iosfwd>
#include <string>

#include "xiva/forward.hpp"

namespace xiva { namespace details {

class websocket_info {

public:
	websocket_info();

	bool empty() const;
	bool valid() const;

	bool parse(request_impl const &req, bool secure);

	void write_message(std::ostream &stream, std::string const &msg) const;

	void write_headers(std::ostream &stream) const;
	void write_body(std::ostream &stream) const;

	bool write_close(std::ostream &stream) const;

private:
	websocket_info(websocket_info const &);
	websocket_info& operator = (websocket_info const &);

	void parse_sec_key(request_impl const &req);
	void parse_sec_keys(request_impl const &req);

private:
	std::string origin_;
	std::string protocol_;
	std::string location_;
	std::string sec_data_;
	bool empty_;
	bool proto_78_;
};

inline bool
websocket_info::empty() const {
	return empty_;
}

inline bool
websocket_info::valid() const {
	return !location_.empty();
}

}} // namespaces

#endif // XIVA_DETAILS_WEBSOCKET_INFO_HPP_INCLUDED
