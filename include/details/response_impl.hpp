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

#ifndef XIVA_DETAILS_RESPONSE_IMPL_HPP_INCLUDED
#define XIVA_DETAILS_RESPONSE_IMPL_HPP_INCLUDED

#include <string>
#include <map>

#include "xiva/shared.hpp"

namespace xiva { namespace details {

class response_impl {

public:
	response_impl();
	virtual ~response_impl();

	void swap(response_impl &impl) throw ();

	std::string const& content_type() const;
	void content_type(std::string const &type);

	std::string const& default_formatter_id() const;
	void formatter_id(std::string const &fmt_id);

	std::map<std::string, std::string> const& channels_data() const;
	void formatter_by_channel(std::string const &channel_id, std::string const &fmt_id);

	bool single_message() const;
	void single_message(bool value);

	std::string const* content_ptr() const;
	void content(std::string const &body);
	void content(std::string const *body);

private:
	response_impl(response_impl const &);
	response_impl& operator = (response_impl const &);

private:
	std::string type_;
	std::string formatter_id_;
	std::map<std::string, std::string> channels_data_;
	std::string content_;
	std::string const *content_ptr_;
	bool single_message_;
};

inline std::string const&
response_impl::content_type() const {
	return type_;
}

inline void
response_impl::content_type(std::string const &type) {
	// TODO check for correctness of type
	type_.assign(type);
}

inline std::string const&
response_impl::default_formatter_id() const {
	return formatter_id_;
}

inline void
response_impl::formatter_id(std::string const &fmt_id) {
	formatter_id_.assign(fmt_id);
}

inline std::map<std::string, std::string> const&
response_impl::channels_data() const {
	return channels_data_;
}

inline bool
response_impl::single_message() const {
	return single_message_;
}

inline void
response_impl::single_message(bool value) {
	single_message_ = value;
}

inline std::string const*
response_impl::content_ptr() const {
	return content_ptr_;
}

inline void
response_impl::content(std::string const &body) {
	content_.assign(body);
	content_ptr_ = &content_;
}

inline void
response_impl::content(std::string const *body) {
	content_ptr_ = body;
}


}} // namespace

#endif // XIVA_DETAILS_RESPONSE_IMPL_HPP_INCLUDED
