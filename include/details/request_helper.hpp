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

#ifndef XIVA_DETAILS_REQUEST_HELPER_HPP_INCLUDED
#define XIVA_DETAILS_REQUEST_HELPER_HPP_INCLUDED

#include <string>
#include <iterator>

#include "xiva/http_error.hpp"

#include "details/range.hpp"
#include "details/functors.hpp"
#include "details/line_reader.hpp"
#include "details/http_constants.hpp"
#include "details/string_utils.hpp"

namespace xiva { namespace details {

template <typename Iter>
struct request_helper {

	static range<Iter>
	parse_request_line(range<Iter> const &line) {

		typedef typename std::iterator_traits<Iter>::value_type char_type;

		is_blank<char_type> space_check;
		range<Iter> r = line, method, protocol, uri;

		split_if_once(r, space_check, method, r);
		if (!is_ci_equal(http_constants<char_type>::get, method)) {
			throw http_error(http_error::method_not_allowed);
		}

		split_if_once(r, space_check, uri, r);

		split_if_once(r, space_check, protocol, r);
		if (!is_ci_equal(http_constants<char_type>::protocol_version, trim(protocol))) {
			throw http_error(http_error::version_not_supported);
		}
		return uri;
	}

	static bool
	find_headers_break(range<Iter> const &end_line, Iter &break_ptr) {

		typedef typename std::iterator_traits<Iter>::value_type char_type;
		typedef typename range<Iter>::size_type size_type;

		size_type sz = end_line.size();

		if (sz > http_constants<char_type>::nonstd_headers_end.size()) {
			if (starts_with(end_line, http_constants<char_type>::nonstd_headers_end)) {
				break_ptr = end_line.begin();
				std::advance(break_ptr, http_constants<char_type>::nonstd_headers_end.size());
				return true;
			}
			if (starts_with(end_line, http_constants<char_type>::headers_end)) {
				break_ptr = end_line.begin();
				std::advance(break_ptr, http_constants<char_type>::headers_end.size());
				return true;
			}
		}
		else if (sz == http_constants<char_type>::nonstd_headers_end.size() &&
			std::equal(http_constants<char_type>::nonstd_headers_end.begin(),
				http_constants<char_type>::nonstd_headers_end.end(), end_line.begin())) {
			break_ptr = end_line.end();
			return true;
		}
		return false;
	}

	static int
	check_read_body_ahead(range<Iter> const &line) {

		typedef typename std::iterator_traits<Iter>::value_type char_type;

		enum { SEC_WEBSOCKET_BODY_SIZE = 8 };

		range<Iter> head, tail;
		if (split_once(line, ':', head, tail)) {
			head = trim_right(head);
			if (is_ci_equal(head, http_constants<char_type>::sec_websocket_key1)) {
				return SEC_WEBSOCKET_BODY_SIZE;
			}
		}
		return 0;
	}
};

}} // namespaces

#endif // XIVA_DETAILS_REQUEST_HELPER_HPP_INCLUDED
