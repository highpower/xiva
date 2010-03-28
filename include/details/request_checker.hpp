/** @file request_checker.hpp */
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
#include "details/range.hpp"
#include "details/string_utils.hpp"
#include "details/http_constants.hpp"
#include "details/iterator_checker.hpp"

namespace xiva { namespace details {

struct http_request_checker {
	template <typename Iter> std::pair<Iter, bool> operator () (Iter first, Iter second) const;
};

struct websocket_request_checker {
	template <typename Iter> std::pair<Iter, bool> operator () (Iter first, Iter second) const;
};

template <typename Iter> inline std::pair<Iter, bool>
http_request_checker::operator () (Iter first, Iter last) const {

	typedef std::reverse_iterator<Iter> iterator_type;
	typedef typename std::iterator_traits<Iter>::value_type char_type;
	BOOST_STATIC_ASSERT(sizeof(typename std::iterator_traits<Iter>::value_type) == 1);

	iterator_checker<Iter> iter_check;
	(void) iter_check;

	iterator_type begin(last), end(first);
	iterator_type pos = std::find(begin, end, static_cast<char_type>('\n'));
	if (end != pos) {
		range<iterator_type> range(pos, end);
		if (starts_with(range, http_constants<char_type>::reversed_headers_end)) {
			std::advance(pos, http_constants<char_type>::reversed_headers_end.size());
			return std::make_pair(pos.base(), true);
		}
		else if (starts_with(range, http_constants<char_type>::reversed_nonstd_headers_end)) {
			std::advance(pos, http_constants<char_type>::reversed_nonstd_headers_end.size());
			return std::make_pair(pos.base(), true);
		}
	}
	return std::make_pair(last, false);
}

template <typename Iter> inline std::pair<Iter, bool>
websocket_request_checker::operator () (Iter first, Iter last) const {

	typedef std::reverse_iterator<Iter> iterator_type;
	typedef typename std::iterator_traits<Iter>::value_type char_type;
	BOOST_STATIC_ASSERT(sizeof(typename std::iterator_traits<Iter>::value_type) == 1);

	iterator_checker<Iter> iter_check;
	(void) iter_check;

	iterator_type begin(last), end(first);
	if ((first != last) && (static_cast<char_type>('<') == *first)) {
		return std::make_pair(last, (static_cast<char_type>(0) == *begin));
	}
	return std::make_pair(last, false);
}

class request_checker {

public:
	request_checker();
	template <typename Iter> std::pair<Iter, bool> operator () (Iter first, Iter second) const;

private:
	http_request_checker http_checker_;
	websocket_request_checker websocket_checker_;
};

request_checker::request_checker() 
{
}

template <typename Iter> inline std::pair<Iter, bool>
request_checker::operator () (Iter first, Iter last) const {
	std::pair<Iter, bool> result = websocket_checker_(first, last);
	return (result.second) ? result : http_checker_(first, last);
}

}} // namespaces

XIVA_BEGIN_ASIO_NAMESPACE

template <>
struct is_match_condition<xiva::details::request_checker> : public boost::true_type {};

XIVA_END_ASIO_NAMESPACE

#endif // XIVA_DETAILS_REQUEST_CHECKER_HPP_INCLUDED
