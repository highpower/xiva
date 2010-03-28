/** @file range.hpp */
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

#ifndef XIVA_DETAILS_RANGE_HPP_INCLUDED
#define XIVA_DETAILS_RANGE_HPP_INCLUDED

#include <cstring>
#include <iterator>
#include <algorithm>
#include <functional>

namespace xiva { namespace details {

template <typename Iter, typename Tag>
class range_base;

template <typename Iter>
class range_base<Iter, std::forward_iterator_tag> {

public:
	range_base();
	range_base(Iter begin, Iter end);

	typedef Iter iterator;
	typedef Iter const_iterator;
	typedef typename std::iterator_traits<Iter>::value_type value_type;
	typedef typename std::iterator_traits<Iter>::difference_type size_type;

	iterator end();
	const_iterator end() const;

	iterator begin();
	const_iterator begin() const;

	void clear();
	bool empty() const;
	size_type size() const;

private:
	Iter begin_, end_;
};

template <typename Iter>
class range_base<Iter, std::bidirectional_iterator_tag> : public range_base<Iter, std::forward_iterator_tag> {

public:
	range_base();
	range_base(Iter begin, Iter end);

	typedef std::reverse_iterator<Iter> reverse_iterator;
	typedef std::reverse_iterator<Iter> const_reverse_iterator;

	reverse_iterator rend();
	const_reverse_iterator rend() const;

	reverse_iterator rbegin();
	const_reverse_iterator rbegin() const;
};

template <typename Iter>
class range_base<Iter, std::random_access_iterator_tag> : public range_base<Iter, std::bidirectional_iterator_tag> {

public:
	range_base();
	range_base(Iter begin, Iter end);

	typedef typename std::iterator_traits<Iter>::value_type value_type;
	typedef typename std::iterator_traits<Iter>::difference_type size_type;

	value_type& operator [] (size_type index);
	value_type const& operator [] (size_type index) const;
};

template <typename Iter>
class range : public range_base<Iter, typename std::iterator_traits<Iter>::iterator_category> {

public:
	range();
	range(Iter begin, Iter end);
};

template <typename Iter> inline
range_base<Iter, std::forward_iterator_tag>::range_base() :
	begin_(), end_() 
{
}

template <typename Iter> inline
range_base<Iter, std::forward_iterator_tag>::range_base(Iter begin, Iter end) :
	begin_(begin), end_(end)
{
}

template <typename Iter> inline typename range_base<Iter, std::forward_iterator_tag>::iterator
range_base<Iter, std::forward_iterator_tag>::end() {
	return end_;
}

template <typename Iter> inline typename range_base<Iter, std::forward_iterator_tag>::const_iterator
range_base<Iter, std::forward_iterator_tag>::end() const {
	return end_;
}

template <typename Iter> inline typename range_base<Iter, std::forward_iterator_tag>::iterator
range_base<Iter, std::forward_iterator_tag>::begin() {
	return begin_;
}

template <typename Iter> inline typename range_base<Iter, std::forward_iterator_tag>::const_iterator
range_base<Iter, std::forward_iterator_tag>::begin() const {
	return begin_;
}

template <typename Iter> inline void
range_base<Iter, std::forward_iterator_tag>::clear() {
	begin_ = end_;
}

template <typename Iter> inline bool
range_base<Iter, std::forward_iterator_tag>::empty() const {
	return begin_ == end_;
}

template <typename Iter> inline typename range_base<Iter, std::forward_iterator_tag>::size_type
range_base<Iter, std::forward_iterator_tag>::size() const {
	return std::distance(begin_, end_);
}

template <typename Iter> inline
range_base<Iter, std::bidirectional_iterator_tag>::range_base() :
	range_base<Iter, std::forward_iterator_tag>()
{
}

template <typename Iter> inline
range_base<Iter, std::bidirectional_iterator_tag>::range_base(Iter begin, Iter end) :
	range_base<Iter, std::forward_iterator_tag>(begin, end) 
{
}

template <typename Iter> inline typename range_base<Iter, std::bidirectional_iterator_tag>::reverse_iterator
range_base<Iter, std::bidirectional_iterator_tag>::rend() {
	return reverse_iterator(this->begin());
}

template <typename Iter> inline typename range_base<Iter, std::bidirectional_iterator_tag>::const_reverse_iterator
range_base<Iter, std::bidirectional_iterator_tag>::rend() const {
	return const_reverse_iterator(this->begin());
}

template <typename Iter> inline typename range_base<Iter, std::bidirectional_iterator_tag>::reverse_iterator
range_base<Iter, std::bidirectional_iterator_tag>::rbegin() {
	return reverse_iterator(this->end());
}

template <typename Iter> inline typename range_base<Iter, std::bidirectional_iterator_tag>::const_reverse_iterator
range_base<Iter, std::bidirectional_iterator_tag>::rbegin() const {
	return const_reverse_iterator(this->end());
}

template <typename Iter> inline
range_base<Iter, std::random_access_iterator_tag>::range_base() :
	range_base<Iter, std::bidirectional_iterator_tag>() 
{
}

template <typename Iter> inline
range_base<Iter, std::random_access_iterator_tag>::range_base(Iter begin, Iter end) :
	range_base<Iter, std::bidirectional_iterator_tag>(begin, end) 
{
}

template <typename Iter> inline typename range_base<Iter, std::random_access_iterator_tag>::value_type&
range_base<Iter, std::random_access_iterator_tag>::operator [] (typename range_base<Iter, typename std::random_access_iterator_tag>::size_type index) {
	return *(this->begin() + index);
}

template <typename Iter> inline typename range_base<Iter, std::random_access_iterator_tag>::value_type const&
range_base<Iter, std::random_access_iterator_tag>::operator [] (typename range_base<Iter, typename std::random_access_iterator_tag>::size_type index) const {
	return *(this->begin() + index);
}

template <typename Iter> inline
range<Iter>::range() :
	range_base<Iter, typename std::iterator_traits<Iter>::iterator_category>() 
{
}

template <typename Iter> inline
range<Iter>::range(Iter begin, Iter end) :
	range_base<Iter, typename std::iterator_traits<Iter>::iterator_category>(begin, end)
{
}

template <typename Iter, typename Other> inline bool
operator < (range<Iter> const &r, range<Other> const &other) {
	return std::lexicographical_compare(r.begin(), r.end(), other.begin(), other.end());
}

template <typename Iter, typename Other> inline bool
operator > (range<Iter> const &r, range<Other> const &other) {
	std::greater<typename std::iterator_traits<Iter>::value_type> pred;
	return std::lexicographical_compare(r.begin(), r.end(), other.begin(), other.end(), pred);
}

template <typename Iter, typename Other> inline bool
operator == (range<Iter> const &r, range<Other> const &other) {
	if (r.size() == other.size()) {
		return std::equal(r.begin(), r.end(), other.begin());
	}
	return false;
}

template <typename Iter, typename Other> inline bool
operator != (range<Iter> const &r, range<Other> const &other) {
	return !operator == (r, other);
}

template <typename Iter, typename Other> inline bool
operator <= (range<Iter> const &r, range<Other> const &other) {
	return !operator > (r, other);
}

template <typename Iter, typename Other> inline bool
operator >= (range<Iter> const &r, range<Other> const &other) {
	return !operator < (r, other);
}

inline range<char const*>
make_range(char const *str) {
	return range<char const*>(str, str + strlen(str));
}

template <typename Sequence> inline range<typename Sequence::const_iterator>
make_range(Sequence const &seq) {
	return range<typename Sequence::const_iterator>(seq.begin(), seq.end());
}

template <typename Iter> inline range<Iter>
make_range(Iter begin, Iter end) {
	return range<Iter>(begin, end);
}

}} // namespaces

#endif // XIVA_DETAILS_RANGE_HPP_INCLUDED
