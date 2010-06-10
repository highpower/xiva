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

#ifndef XIVA_DETAILS_HTTP_CONSTANTS_HPP_INCLUDED
#define XIVA_DETAILS_HTTP_CONSTANTS_HPP_INCLUDED

#include <string>

#include "details/range.hpp"

namespace xiva { namespace details {

template <typename Char>
struct http_constants;

template <>
struct http_constants<const char> {

	static char const* const endl;
	static std::string const empty_string;

	static range<char const*> const get;
	static range<char const*> const range_name;
	static range<char const*> const cookie;
	static range<char const*> const param_delimiter;
	static range<char const*> const protocol_version;
	static range<char const*> const policy_file_request;

	static range<char const*> const reversed_headers_end;
	static range<char const*> const reversed_nonstd_headers_end;
};

template <>
struct http_constants<char> {

        static char const* const endl;
        static std::string const empty_string;

        static range<char const*> const get;
        static range<char const*> const range_name;
        static range<char const*> const cookie;
        static range<char const*> const param_delimiter;
        static range<char const*> const protocol_version;
        static range<char const*> const policy_file_request;

        static range<char const*> const reversed_headers_end;
        static range<char const*> const reversed_nonstd_headers_end;
};


}} // namespaces

#endif // XIVA_DETAILS_HTTP_CONSTANTS_HPP_INCLUDED
