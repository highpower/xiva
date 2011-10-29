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

#ifndef XIVA_RESPONSE_HPP_INCLUDED
#define XIVA_RESPONSE_HPP_INCLUDED

#include <string>

#include "xiva/config.hpp"
#include "xiva/forward.hpp"

namespace xiva {

class XIVA_API response {

public:
	response(details::response_impl &impl);
	virtual ~response();

	void content_type(char const *type);
	void content_type(std::string const &type);

	void formatter_id(std::string const &fmt_id);

	void formatter_by_channel(channel_info const &ch_info, std::string const &fmt_id);

	void set_header(std::string const &name, std::string const &value);

	void single_message(bool value);

	void content(std::string const &body);
	void content(std::string const *body);

private:
	response(response const &);
	response& operator = (response const &);

private:
	details::response_impl &impl_;
};

} // namespace

#endif // XIVA_RESPONSE_HPP_INCLUDED
