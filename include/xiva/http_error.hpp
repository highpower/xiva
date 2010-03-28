/** @file http_error.hpp */
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

#ifndef XIVA_HTTP_ERROR_HPP_INCLUDED
#define XIVA_HTTP_ERROR_HPP_INCLUDED

#include <exception>

#include "xiva/config.hpp"

namespace xiva {

class XIVA_API http_error : public std::exception {

public:
	http_error(unsigned short code);

	virtual ~http_error() throw ();
	virtual char const* what() const throw ();
	unsigned short code() const;

	static unsigned short const bad_request;
	static unsigned short const unauthorized;
	static unsigned short const payment_required;
	static unsigned short const forbidden;
	static unsigned short const not_found;
	static unsigned short const method_not_allowed;
	static unsigned short const entity_too_large;
	static unsigned short const range_not_satisfiable;

	static unsigned short const internal_error;
	static unsigned short const not_implemented;
	static unsigned short const bad_gateway;
	static unsigned short const service_unavailable;
	static unsigned short const gateway_timeout;
	static unsigned short const version_not_supported;

private:
	unsigned short code_;
};

} // namespace

#endif // XIVA_HTTP_ERROR_HPP_INCLUDED
