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

#ifndef XIVA_DETAILS_REQUEST_CHECKER_HPP_INCLUDED
#define XIVA_DETAILS_REQUEST_CHECKER_HPP_INCLUDED

#include <utility>
#include <iterator>
#include <algorithm>

#include <boost/type_traits.hpp>
#include <boost/static_assert.hpp>

#include "details/asio.hpp"
#include "details/functors.hpp"
#include "details/line_reader.hpp"
#include "details/range.hpp"
#include "details/string_utils.hpp"
#include "details/http_constants.hpp"
#include "details/iterator_checker.hpp"
#include "details/request_helper.hpp"

namespace xiva { namespace details {

struct http_request_checker {
	template <typename Iter> std::pair<Iter, bool> operator () (Iter first, Iter second) const;
};

struct request_policy_checker {
	template <typename Iter> std::pair<Iter, bool> operator () (Iter first, Iter second) const;
};

template <typename Iter> inline std::pair<Iter, bool>
http_request_checker::operator () (Iter first, Iter last) const {

	typedef typename std::iterator_traits<Iter>::value_type char_type;
	BOOST_STATIC_ASSERT(sizeof(char_type) == sizeof(char));

	range<Iter> line, end_line;
	line_reader<Iter> reader(first, last);
	if (reader.read_line(line, end_line) && !end_line.empty()) {

		int read_body_ahead = 0;
		request_helper<Iter> helper;

		helper.parse_request_line(line);
		while (reader.read_line(line, end_line)) {
			int res = helper.check_read_body_ahead(line);
			if (res > 0) {
				read_body_ahead = res;
			}
			Iter break_ptr;
			if (helper.find_headers_break(end_line, break_ptr)) {
				if (!read_body_ahead) {
					return std::make_pair(end_line.begin(), true);
				}
				if (std::distance(break_ptr, last) >= read_body_ahead) {
					return std::make_pair(last, true);
				}
			}
		}
	}

	return std::make_pair(last, false);
}

template <typename Iter> inline std::pair<Iter, bool>
request_policy_checker::operator () (Iter first, Iter last) const {

	typedef std::reverse_iterator<Iter> iterator_type;
	typedef typename std::iterator_traits<Iter>::value_type char_type;
	BOOST_STATIC_ASSERT(sizeof(char_type) == sizeof(char));

	iterator_checker<Iter> iter_check;
	(void) iter_check;

	iterator_type begin(last), end(first);
	if (static_cast<char_type>('<') == *first) {
		return std::make_pair(last, (static_cast<char_type>(0) == *begin));
	}
	return std::make_pair(last, false);
}

class request_checker {

public:
	explicit request_checker(int max_size);
	template <typename Iter> std::pair<Iter, bool> operator () (Iter first, Iter second) const;

private:
	http_request_checker http_checker_;
	request_policy_checker policy_checker_;
	int max_size_;
};

request_checker::request_checker(int max_size) : max_size_(max_size)
{
}

template <typename Iter> inline std::pair<Iter, bool>
request_checker::operator () (Iter first, Iter last) const {

	typedef typename std::iterator_traits<Iter>::difference_type diff_type;
	diff_type size = std::distance(first, last);

	enum { min_size = sizeof("get / http/1.1\n\n") - 1 };
	// sizeof ("<policy-file-request/>\0") - 1 > min_size

	if (size < min_size) {
		return std::make_pair(last, false);
	}
	if (size > (diff_type)max_size_) {
		return std::make_pair(last, true); // will be handled in connection_impl
	}
	try {
		std::pair<Iter, bool> result = policy_checker_(first, last);
		return (result.second) ? result : http_checker_(first, last);
	}
	catch (...) {
		return std::make_pair(last, true); // will be handled in request_impl
	}
}

}} // namespaces

XIVA_BEGIN_ASIO_NAMESPACE

template <>
struct is_match_condition<xiva::details::request_checker> : public boost::true_type {};

XIVA_END_ASIO_NAMESPACE

#endif // XIVA_DETAILS_REQUEST_CHECKER_HPP_INCLUDED
