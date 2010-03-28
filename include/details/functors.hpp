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


#ifndef XIVA_DETAILS_FUNCTORS_HPP_INCLUDED
#define XIVA_DETAILS_FUNCTORS_HPP_INCLUDED

#include <functional>
#include <boost/type_traits.hpp>
#include <boost/static_assert.hpp>

#include "details/char_traits.hpp"

namespace xiva { namespace details {

template <typename Pred, typename T>
struct unary_predicate : public std::unary_function<T, bool> {
	bool operator () (T var) const;
};

template <typename Pred, typename T>
struct binary_predicate : public std::binary_function<T const&, T const&, bool> {
	bool operator () (T const &var, T const &target) const;
};

template <typename Char>
struct is_space : public unary_predicate<is_space<Char>, Char> {
	static bool check(Char value);
};

template <typename Char>
struct is_line_end : public unary_predicate<is_line_end<Char>, Char> {
	static bool check(Char value);
};

template <typename Char>
struct is_not_line_end : public unary_predicate<is_not_line_end<Char>, Char> {
	static bool check(Char value);
};

template <typename Range>
struct ci_less : public binary_predicate<ci_less<Range>, Range> {
	static bool check(Range const &var, Range const &target);
};

template <typename Range>
struct ci_equal : public binary_predicate<ci_equal<Range>, Range> {
	static bool check(Range const &var, Range const &target);
};

template <>
struct ci_less<char> : public std::binary_function<char, char, bool> {
	bool operator () (char var, char target) const;
};

template <>
struct ci_equal<char> : public std::binary_function<char, char, bool> {
	bool operator () (char var, char target) const;
};

template <typename R1, typename R2> inline bool
is_ci_less(R1 const &var, R2 const &target) {
	BOOST_STATIC_ASSERT((boost::is_same<typename R1::value_type, typename R2::value_type>::value));
	return std::lexicographical_compare(var.begin(), var.end(), target.begin(),
		target.end(), ci_less<typename R1::value_type>());
}

template <typename R1, typename R2> inline bool
is_ci_equal(R1 const &var, R2 const &target) {
	BOOST_STATIC_ASSERT((boost::is_same<typename R1::value_type, typename R2::value_type>::value));
	if (var.size() == target.size()) {
		return std::equal(var.begin(), var.end(), target.begin(), 
			ci_equal<typename R1::value_type>());
	}
	return false;
}

template <typename Pred, typename T> inline bool
unary_predicate<Pred, T>::operator () (T value) const {
	return Pred::check(value);
}

template <typename Pred, typename T> inline bool
binary_predicate<Pred, T>::operator () (T const &var, T const &target) const {
	return Pred::check(var, target);
}

template <typename Char> inline bool
is_space<Char>::check(Char value) {
	return char_traits<Char>::is_space(value);
}

template <typename Char> inline bool
is_line_end<Char>::check(Char value) {
	return (static_cast<Char>('\n') == value || static_cast<Char>('\r') == value);
}

template <typename Char> inline bool
is_not_line_end<Char>::check(Char value) {
	return !is_line_end<Char>::check(value);
}

template <typename Range> inline bool
ci_less<Range>::check(Range const &var, Range const &target) {
	return is_ci_less(var, target);
}

template <typename Range> inline bool
ci_equal<Range>::check(Range const &var, Range const &target) {
	return is_ci_equal(var, target);
}

inline bool
ci_less<char>::operator () (char var, char target) const {
	return char_traits<char>::to_lower(var) < char_traits<char>::to_lower(target);
}

inline bool
ci_equal<char>::operator () (char var, char target) const {
	return char_traits<char>::to_lower(var) == char_traits<char>::to_lower(target);
}

}} // namespaces

#endif // XIVA_DETAILS_FUNCTORS_HPP_INCLUDED
