/** @file websocket_info.hpp */
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

namespace xiva { namespace details {


class websocket_info;
std::ostream& operator << (std::ostream &stream, websocket_info const &val);

class websocket_info {

public:
	websocket_info();
	friend std::ostream& operator << (std::ostream &stream, websocket_info const &val);

	bool empty() const;
	bool valid() const;

	void parse(request const &req);

	static void write_message(std::ostream &stream, std::string const &msg);

private:
	websocket_info(websocket_info const &);
	websocket_info& operator = (websocket_info const &);

	std::string origin_;
	std::string protocol_;
	std::string location_;
	bool empty_;
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
