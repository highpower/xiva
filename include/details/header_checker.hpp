/** @file header_checker.hpp */
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

#ifndef XIVA_DETAILS_HEADER_CHECKER_HPP_INCLUDED
#define XIVA_DETAILS_HEADER_CHECKER_HPP_INCLUDED

#include <utility>
#include <iterator>
#include <algorithm>

#include "details/asio.hpp"
#include <boost/type_traits.hpp>
#include <boost/static_assert.hpp>

#include "string_utils.hpp"
#include "details/range.hpp"
#include "details/http_constants.hpp"
#include "details/iterator_checker.hpp"

namespace xiva { namespace details {

struct header_checker {
	template <typename Iter> std::pair<Iter, bool> operator () (Iter first, Iter second) const;
};

template <typename Iter> inline std::pair<Iter, bool>
header_checker::operator () (Iter first, Iter last) const {
	
	typedef std::reverse_iterator<Iter> iterator_type;
	
	iterator_checker<Iter> iter_check;
	(void) iter_check;
	
	BOOST_STATIC_ASSERT(sizeof(typename std::iterator_traits<Iter>::value_type) == 1);
	
	iterator_type begin(last), end(first);
	iterator_type pos = std::find(begin, end, '\n');
	if (end != pos) {
		range<iterator_type> range(pos, end);
		if (starts_with(range, http_constants::reversed_headers_end)) {
			std::advance(pos, http_constants::reversed_headers_end.size());
			return std::make_pair(pos.base(), true);
		}
		else if (starts_with(range, http_constants::reversed_nonstd_headers_end)) {
			std::advance(pos, http_constants::reversed_nonstd_headers_end.size());
			return std::make_pair(pos.base(), true);
		}
	}
	return std::make_pair(last, false);
}

}} // namespaces


#if XIVA_HAVE_BOOST_ASIO
namespace boost {
#endif // XIVA_HAVE_BOOST_ASIO

namespace asio {

template <>
struct is_match_condition<xiva::details::header_checker> : public boost::true_type {};

} // namespace asio

#if XIVA_HAVE_BOOST_ASIO
}
#endif // XIVA_HAVE_BOOST_ASIO

#endif // XIVA_DETAILS_HTTP_COMPLETION_CHECKER_HPP_INCLUDED
