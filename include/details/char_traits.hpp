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

#ifndef XIVA_DETAILS_CHAR_TRAITS_HPP_INCLUDED
#define XIVA_DETAILS_CHAR_TRAITS_HPP_INCLUDED

#include <cctype>

namespace xiva { namespace details {

template <typename Char>
struct char_traits;

template <>
struct char_traits<char> {

	static bool is_space(char value);
	static bool is_alpha(char value);
	static bool is_alpha_numeric(char value);

	static char to_upper(char value);
	static char to_lower(char value);
};

inline bool
char_traits<char>::is_space(char value) {
	return isspace(value);
}

inline bool
char_traits<char>::is_alpha(char value) {
	return isalpha(value);
}

inline bool
char_traits<char>::is_alpha_numeric(char value) {
	return isalnum(value);
}

inline char
char_traits<char>::to_upper(char value) {
	return toupper(value);
}

inline char
char_traits<char>::to_lower(char value) {
	return tolower(value);
}


template <>
struct char_traits<const char> {

        static bool is_space(const char value);
        static bool is_alpha(const char value);
        static bool is_alpha_numeric(const char value);

        static const char to_upper(const char value);
        static const char to_lower(const char value);
};

inline bool
char_traits<const char>::is_space(const char value) {
        return isspace(value);
}

inline bool
char_traits<const char>::is_alpha(const char value) {
        return isalpha(value);
}

inline bool
char_traits<const char>::is_alpha_numeric(const char value) {
        return isalnum(value);
}

inline const char
char_traits<const char>::to_upper(const char value) {
        return toupper(value);
}

inline const char
char_traits<const char>::to_lower(const char value) {
        return tolower(value);
}



}} // namespaces

#endif // XIVA_DETAILS_CHAR_TRAITS_HPP_INCLUDED
