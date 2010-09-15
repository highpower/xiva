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

#ifndef XIVA_DETAILS_LINE_READER_HPP_INCLUDED
#define XIVA_DETAILS_LINE_READER_HPP_INCLUDED

#include <iterator>
#include <algorithm>

#include "details/range.hpp"
#include "details/functors.hpp"
#include "details/string_utils.hpp"
#include "details/iterator_checker.hpp"

namespace xiva { namespace details {

template <typename Iter>
class line_reader : private iterator_checker<Iter> {

public:
	line_reader(Iter begin, Iter end);
	bool multiline() const;
	bool read_line(range<Iter> &line, range<Iter> &end_line);

private:
	line_reader(line_reader const &);
	line_reader& operator = (line_reader const &);
	typedef typename std::iterator_traits<Iter>::value_type char_type;

private:
	Iter begin_;
	Iter const end_;
	bool multiline_;
	is_blank<char_type> space_check_;
	is_line_end<char_type> line_end_check_;
};

template <typename Iter> inline
line_reader<Iter>::line_reader(Iter begin, Iter end) :
	begin_(begin), end_(end), multiline_(false) 
{
}


template <typename Iter> inline bool
line_reader<Iter>::multiline() const {
	return multiline_;
}

template <typename Iter> inline bool
line_reader<Iter>::read_line(range<Iter> &line, range<Iter> &end_line) {

	if (end_ == begin_) return false;

	multiline_ = false;
	Iter position = begin_;

	while (true) {

		Iter line_end = std::find_if(position, end_, line_end_check_);
		Iter newline_begin = find_if_not(line_end, end_, line_end_check_);
		line = range<Iter>(begin_, line_end);
		end_line = range<Iter>(line_end, newline_begin);

		if (end_ == line_end || end_ == newline_begin) {
			begin_ = end_;
			return true;
		}
		if (!space_check_(*newline_begin)) {
			begin_ = newline_begin;
			return true;
		}
		multiline_ = true;
		position = newline_begin;
	}
}

}} // namespaces

#endif // XIVA_DETAILS_LINE_READER_HPP_INCLUDED
