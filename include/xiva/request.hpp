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

#ifndef XIVA_REQUEST_HPP_INCLUDED
#define XIVA_REQUEST_HPP_INCLUDED

#include <string>

#include "xiva/config.hpp"
#include "xiva/enumeration.hpp"

namespace xiva {

class XIVA_API request {

public:
	request();
	virtual ~request();

	virtual std::string const& url() const = 0;

	virtual bool has_param(std::string const &name) const = 0;
	virtual std::string const& param(std::string const &name) const = 0;
	virtual enumeration<std::string>::ptr_type params(std::string const &name) const = 0;

	virtual bool has_header(std::string const &name) const = 0;
	virtual std::string const& header(std::string const &name) const = 0;

	virtual bool has_cookie(std::string const &name) const = 0;
	virtual std::string const& cookie(std::string const &name) const = 0;

private:
	request(request const &);
	request& operator = (request const &);
};

} // namespace

#endif // XIVA_REQUEST_HPP_INCLUDED
