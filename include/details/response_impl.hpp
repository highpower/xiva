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

#include "xiva/shared.hpp"

namespace xiva { namespace details {

class response_impl {

public:
	response_impl();
	virtual ~response_impl();

	void swap(response_impl &impl) throw ();

	std::string const& content_type() const;
	void content_type(std::string const &type);

	std::string const& formatter_id() const;
	void formatter_id(std::string const &fmt_id);

	bool single_message() const;
	void single_message(bool value);

private:
	response_impl(response_impl const &);
	response_impl& operator = (response_impl const &);

private:
	std::string type_;
	std::string formatter_id_;
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
response_impl::formatter_id() const {
	return formatter_id_;
}

inline void
response_impl::formatter_id(std::string const &fmt_id) {
	formatter_id_.assign(fmt_id);
}

inline bool
response_impl::single_message() const {
	return single_message_;
}

inline void
response_impl::single_message(bool value) {
	single_message_ = value;
}


}} // namespace

#endif // XIVA_DETAILS_RESPONSE_IMPL_HPP_INCLUDED
