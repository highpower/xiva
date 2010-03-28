/** @file string_utils.hpp */
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

#ifndef XIVA_DETAILS_STRING_UTILS_HPP_INCLUDED
#define XIVA_DETAILS_STRING_UTILS_HPP_INCLUDED

#include <string>
#include <boost/type_traits.hpp>
#include <boost/static_assert.hpp>

#include "details/range.hpp"
#include "details/functors.hpp"

namespace xiva { namespace details {

template <typename Iter, typename T> inline Iter
find_not(Iter begin, Iter end, T const &value) {
	for (Iter i = begin; i != end; ++i) {
		if (value != (*i)) return i;
	}
	return end;
}

template <typename Iter, typename Pred> inline Iter
find_if_not(Iter begin, Iter end, Pred pred) {
	for (Iter i = begin; i != end; ++i) {
		if (!pred(*i)) return i;
	}
	return end;
}

template <typename Range> inline Range
trim_left(Range const &r) {
	is_space<typename Range::value_type> checker;
	return Range(find_if_not(r.begin(), r.end(), checker), r.end());
}

template <typename Range> inline Range
trim_right(Range const &r) {
	is_space<typename Range::value_type> checker;
	std::reverse_iterator<typename Range::const_iterator> rbegin(r.end()), rend(r.begin());
	return Range(r.begin(), find_if_not(rbegin, rend, checker).base());
}

template <typename Range> inline Range
trim(Range const &r) {
	return trim_left(trim_right(r));
}

template<typename Range> inline Range
truncate(Range const &r, typename Range::size_type left, typename Range::size_type right) {
	typename Range::const_iterator begin = r.begin(), end = r.end();
	while (begin != end && left--) {
		++begin;
	}
	while (begin != end && right--) {
		--end;
	}
	return Range(begin, end);
}

template <typename Range> inline bool
do_split(Range const &r, typename Range::const_iterator pos, typename Range::size_type size, Range &first, Range &second) {

	typename Range::const_iterator begin = r.begin(), end = r.end();
	first = Range(begin, pos);
	if (pos != end) {
		typename Range::const_iterator start = pos;
		std::advance(start, size);
		second = Range(start, end);
		return true;
	}
	second = Range(end, end);
	return false;
}

template <typename Range> inline bool
split_once(Range const &r, typename Range::value_type c, Range &first, Range &second) {
	return do_split(r, std::find(r.begin(), r.end(), c), 1, first, second);
}

template <typename Range, typename Delim> inline bool
split_once(Range const &r, Delim const &delim, Range &first, Range &second, typename Delim::value_type const *sfinae = 0) {
	(void) sfinae;
	BOOST_STATIC_ASSERT((boost::is_same<typename Range::value_type, typename Delim::value_type>::value));
	return do_split(r, std::search(r.begin(), r.end(), delim.begin(), delim.end()), delim.size(), first, second);
}

template <typename Range> inline bool
split_once(Range const &r, typename Range::value_type const *chars, Range &first, Range &second) {
	typedef typename Range::value_type char_type;
	range<char_type const*> delim(chars, chars + std::char_traits<char_type>::length(chars));
	return split_once(r, delim, first, second, static_cast<typename range<typename Range::value_type const*>::value_type const*>(0));
}

template <typename Range, typename Delim> inline bool
split_first_of_once(Range const &r, Delim const &delim, Range &first, Range &second, typename Delim::value_type const *sfinae = 0) {
	(void) sfinae;
	BOOST_STATIC_ASSERT((boost::is_same<typename Range::value_type, typename Delim::value_type>::value));
	return do_split(r, std::find_first_of(r.begin(), r.end(), delim.begin(), delim.end()), 1, first, second);
}

template <typename Range> inline bool
split_first_of_once(Range const &r, typename Range::value_type const *chars, Range &first, Range &second) {
	typedef typename Range::value_type char_type;
	range<char_type const*> delim(chars, chars + std::char_traits<char_type>::length(chars));
	return split_first_of_once(r, delim, first, second, static_cast<typename range<typename Range::value_type const*>::value_type const*>(0));
}

template <typename Range, typename Func> inline bool
split_if_once(Range const &r, Func func, Range &first, Range &second) {
	return do_split(r, std::find_if(r.begin(), r.end(), func), 1, first, second);
}

template <typename Range, typename Prefix> inline bool
starts_with(Range const &r, Prefix const &prefix, typename Prefix::value_type *sfinae = 0) {
	(void) sfinae;
	if (static_cast<std::size_t>(prefix.size()) <= static_cast<std::size_t>(r.size())) {
		return std::equal(prefix.begin(), prefix.end(), r.begin());
	}
	return false;
}

template <typename Range> inline bool
starts_with(Range const &r, char const *prefix) {
	return starts_with(r, range<char const*>(prefix, prefix + std::char_traits<char>::length(prefix)));
}

template <typename Range> inline bool
starts_with(char const *string, Range const &prefix) {
	return starts_with(range<char const*>(string, string + std::char_traits<char>::length(string)), prefix);
}

template <typename Range, typename Suffix> inline bool
ends_with(Range const &r, Suffix const &suffix, typename Suffix::value_type *sfinae = 0) {
	(void) sfinae;
	if (static_cast<std::size_t>(suffix.size()) <= static_cast<std::size_t>(r.size())) {
		typename Range::const_iterator i = r.begin();
		std::advance(i, r.size() - suffix.size());
		return std::equal(suffix.begin(), suffix.end(), i);
	}
	return false;
}

template <typename Range> inline bool
ends_with(Range const &r, char const *suffix) {
	return ends_with(r, range<char const*>(suffix, suffix + std::char_traits<char>::length(suffix)));
}

template <typename Range> inline bool
ends_with(char const *string, Range const &suffix) {
	return ends_with(range<char const*>(string, string + std::char_traits<char>::length(string)), suffix);
}

}} // namespaces

#endif // XIVA_DETAILS_STRING_UTILS_HPP_INCLUDED
