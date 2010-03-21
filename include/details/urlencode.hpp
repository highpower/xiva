/** @file urlencode.hpp */
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

#ifndef XIVA_DETAILS_URLENCODE_HPP_INCLUDED
#define XIVA_DETAILS_URLENCODE_HPP_INCLUDED

#include <iterator>
#include <boost/type_traits.hpp>
#include <boost/static_assert.hpp>

#include "details/char_traits.hpp"

namespace xiva { namespace details {

template <typename Result, typename Range> inline Result
urlencode(Range const &range) {
	
	BOOST_STATIC_ASSERT(sizeof(typename Range::value_type) == 1);
	BOOST_STATIC_ASSERT((boost::is_same<typename Range::value_type, typename Result::value_type>::value));
	
	Result result;
	result.reserve(3 * range.size());

	for (typename Range::const_iterator i = range.begin(), end = range.end(); i != end; ++i) {
		if (char_traits<typename Range::value_type>::is_alpha_numeric(*i)) {
			result.push_back(*i);
			continue;
		}
		switch (*i) {
			case static_cast<typename Range::value_type>('-'):
			case static_cast<typename Range::value_type>('_'):
			case static_cast<typename Range::value_type>('.'):
			case static_cast<typename Range::value_type>('!'):
			case static_cast<typename Range::value_type>('~'): 
			case static_cast<typename Range::value_type>('*'):
			case static_cast<typename Range::value_type>('('):
			case static_cast<typename Range::value_type>(')'):
			case static_cast<typename Range::value_type>('\''): 
				result.push_back(*i);
				break;
			default:
				typename Range::value_type bytes[3] = { '%', (*i & 0xF0) / 16, *i & 0x0F };
				for (std::size_t i = 1; i < sizeof(bytes); ++i) {
					bytes[i] += (bytes[i] > 9) ? 'A' - 10 : '0';
				}
				result.insert(result.end(), bytes, bytes + sizeof(bytes));
				break;
		}
	}
	return result;
}

template <typename Result, typename Range> inline Result
urldecode(Range const &range) {
	
	BOOST_STATIC_ASSERT(sizeof(typename Range::value_type) == 1);
	BOOST_STATIC_ASSERT((boost::is_same<typename Range::value_type, typename Result::value_type>::value));

	Result result;
	result.reserve(range.size());

	for (typename Range::const_iterator i = range.begin(), end = range.end(); i != end; ++i) {
		switch (*i) {
			case static_cast<typename Range::value_type>('+'): 
				result.push_back(static_cast<typename Range::value_type>(' '));
				break;
			case static_cast<typename Range::value_type>('%'):
				if (std::distance(i, end) > 2) {
					typename Range::const_iterator f = i, s = i;
					std::advance(f, 1);
					std::advance(s, 2);
					typename Range::value_type first = *f, second = *s;
					int digit = (first >= 'A' ? ((first & 0xDF) - 'A') + 10 : (first - '0')) * 16;
					digit += (second >= 'A') ? ((second & 0xDF) - 'A') + 10 : (second - '0');
					result.push_back(static_cast<typename Range::value_type>(digit));
					std::advance(i, 2);
				}
				else {
					result.push_back('%');
				}
				break;
			default:
				result.push_back(*i);
				break;
		}
	}
	return result;
}

}} // namespaces
	
#endif // XIVA_DETAILS_URLENCODE_HPP_INCLUDED
