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

#ifndef XIVA_DETAILS_REQUEST_IMPL_HPP_INCLUDED
#define XIVA_DETAILS_REQUEST_IMPL_HPP_INCLUDED

#include <map>
#include <string>
#include <boost/iterator/filter_iterator.hpp>

#include "xiva/http_error.hpp"
#include "xiva/enumeration.hpp"

#include "details/range.hpp"
#include "details/functors.hpp"
#include "details/urlencode.hpp"
#include "details/line_reader.hpp"
#include "details/http_constants.hpp"
#include "details/request_helper.hpp"

namespace xiva { namespace details {

class request_impl {

public:
	request_impl();
	template <typename Iter> request_impl(Iter begin, Iter end);
	virtual ~request_impl();

	std::string const& uri() const;
	std::string const& body() const;

	bool has_param(std::string const &name) const;
	std::string const& param(std::string const &name) const;
	enumeration<std::string>::ptr_type params(std::string const &name) const;

	bool has_header(std::string const &name) const;
	std::string const& header(std::string const &name) const;

	bool has_cookie(std::string const &name) const;
	std::string const& cookie(std::string const &name) const;

	bool is_websocket() const;
	void set_websocket();

	void swap(request_impl &other) throw ();

private:
	request_impl(request_impl const &);
	request_impl& operator = (request_impl const &);

	template <typename Iter> void init(Iter begin, Iter end);
	template <typename Iter> void parse_uri(range<Iter> const &uri);
	template <typename Iter> void parse_cookies(range<Iter> const &cookies);
	template <typename Iter> void parse_query_string(range<Iter> const &query);
	template <typename Iter> void parse_request_line(range<Iter> const &line, bool multiline);
	template <typename Iter> void parse_headers_line(range<Iter> const &line, bool multiline);

	template <typename Iter> void parse_request_line_impl(range<Iter> const &line);
	template <typename Iter> void parse_headers_line_impl(range<Iter> const &line);

	template <typename Iter, typename Map> static void parse_to(range<Iter> const &r, Map &map);

	typedef std::pair<std::string const, std::string> param_type;
	typedef std::allocator<param_type> allocator_type;

	typedef std::map<std::string, std::string, ci_less<std::string>, allocator_type> header_map_type;
	typedef std::map<std::string, std::string, std::less<std::string>, allocator_type> cookie_map_type;
	typedef std::multimap<std::string, std::string, std::less<std::string>, allocator_type> param_map_type;

private:
	std::string uri_;
	std::string body_;
	param_map_type params_;
	header_map_type headers_;
	cookie_map_type cookies_;
	bool is_websocket_;
};


inline
request_impl::request_impl() :
	params_(), headers_(), cookies_(), is_websocket_(false)
{
}

template <typename Iter> inline
request_impl::request_impl(Iter begin, Iter end) :
	params_(), headers_(), cookies_(), is_websocket_(false)
{
	init(begin, end);
}

inline std::string const&
request_impl::uri() const {
        return uri_;
}

inline std::string const&
request_impl::body() const {
        return body_;
}

inline bool
request_impl::is_websocket() const {
	return is_websocket_;
}

inline void
request_impl::set_websocket() {
	is_websocket_ = true;
}

template <typename Iter> inline void
request_impl::init(Iter begin, Iter end) {

	typedef typename std::iterator_traits<Iter>::value_type char_type;

	request_helper<Iter> helper;
	range<Iter> line, end_line;
	line_reader<Iter> reader(begin, end);

	if (!reader.read_line(line, end_line)) {
		throw http_error(http_error::bad_request);
	}
	parse_request_line(line, reader.multiline());
	while (reader.read_line(line, end_line)) {
		parse_headers_line(line, reader.multiline());
		Iter break_ptr;
		if (helper.find_headers_break(end_line, break_ptr)) {
			if (break_ptr != end) {
				body_.assign(break_ptr, end);
			}
			break;
		}
	}
}

template <typename Iter> inline void
request_impl::parse_uri(range<Iter> const &uri) {
	Iter begin = uri.begin(), end = uri.end();
	uri_.assign(begin, end);
	Iter i = std::find(begin, end, '?');
	if (end != i) {
		parse_query_string(make_range(++i, end));
	}
}

template <typename Iter> inline void
request_impl::parse_cookies(range<Iter> const &cookies) {
	range<Iter> r = cookies, part;
	while (!r.empty()) {
		split_once(r, ';', part, r);
		parse_to(trim(part), cookies_);
	}
}

template <typename Iter> inline void
request_impl::parse_query_string(range<Iter> const &query) {
	typedef typename std::iterator_traits<Iter>::value_type char_type;
	range<Iter> r = query, part;
	while (!r.empty()) {
		split_first_of_once(r, http_constants<char_type>::param_delimiter, part, r);
		parse_to(part, params_);
	}
}

template <typename Iter> inline void
request_impl::parse_request_line(range<Iter> const &line, bool multiline) {
	if (!multiline) {
		parse_request_line_impl(line);
	}
	else {
		typedef typename std::iterator_traits<Iter>::value_type char_type;
		typedef boost::filter_iterator<is_not_line_end<char_type>, Iter> iterator_type;
		Iter begin = line.begin(), end = line.end();
		range<iterator_type> r(iterator_type(begin, end), iterator_type(end, end));
		parse_request_line_impl(r);
	}
}

template <typename Iter> inline void
request_impl::parse_headers_line(range<Iter> const &line, bool multiline) {
	if (!multiline) {
		parse_headers_line_impl(line);
	}
	else {
		typedef typename std::iterator_traits<Iter>::value_type char_type;
		typedef boost::filter_iterator<is_not_line_end<char_type>, Iter> iterator_type;
		Iter begin = line.begin(), end = line.end();
		range<iterator_type> r(iterator_type(begin, end), iterator_type(end, end));
		parse_headers_line_impl(r);
	}
}

template <typename Iter> inline void
request_impl::parse_request_line_impl(range<Iter> const &line) {

	request_helper<Iter> helper;
	range<Iter> uri = helper.parse_request_line(line);
	parse_uri(uri);
}

template <typename Iter> inline void
request_impl::parse_headers_line_impl(range<Iter> const &line) {

	typedef typename std::iterator_traits<Iter>::value_type char_type;

	range<Iter> head, tail;
	if (!split_once(line, ':', head, tail)) {
		throw http_error(http_error::bad_request);
	}

	tail = trim(tail);
	head = trim_right(head);

	if (is_ci_equal(http_constants<char_type>::range_name, head)) {
		throw http_error(http_error::range_not_satisfiable);
	}
	else if (is_ci_equal(http_constants<char_type>::cookie, head)) {
		parse_cookies(tail);
	}
	headers_.insert(std::make_pair(std::string(head.begin(), head.end()), std::string(tail.begin(), tail.end())));
}

template <typename Iter, typename Map> inline void
request_impl::parse_to(range<Iter> const &r, Map &map) {
	range<Iter> head, tail;
	split_once(r, '=', head, tail);
	map.insert(std::make_pair(urldecode<typename Map::key_type>(head),
		urldecode<typename Map::mapped_type>(tail)));
}

}} // namespaces

#endif // XIVA_DETAILS_REQUEST_IMPL_HPP_INCLUDED
