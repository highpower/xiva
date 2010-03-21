/** @file iterator_checker.hpp */
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

#ifndef XIVA_DETAILS_ITERATOR_CHECKER_HPP_INCLUDED
#define XIVA_DETAILS_ITERATOR_CHECKER_HPP_INCLUDED

#include <iterator>

namespace xiva { namespace details {

template <typename IteratorTag>
struct iterator_tag_checker;

template <>
struct iterator_tag_checker<std::forward_iterator_tag> {
};

template <>
struct iterator_tag_checker<std::bidirectional_iterator_tag> {
};

template <>
struct iterator_tag_checker<std::random_access_iterator_tag> {
};

template <typename Iter>
struct iterator_checker : iterator_tag_checker<typename std::iterator_traits<Iter>::iterator_category> {
};

}} // namespaces

#endif // XIVA_DETAILS_ITERATOR_CHECKER_HPP_INCLUDED
