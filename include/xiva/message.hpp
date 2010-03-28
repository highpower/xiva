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

#ifndef XIVA_MESSAGE_HPP_INCLUDED
#define XIVA_MESSAGE_HPP_INCLUDED

#include <string>

#include "xiva/config.hpp"

namespace xiva {

class XIVA_API message {
public:
	message();
	message(std::string const &value);
	template <typename Iter> message(Iter begin, Iter end);
	virtual ~message();

	std::string const& content() const;
	void content(std::string const &value);

private:
	message(message const &);
	message& operator = (message const &);

private:
	std::string content_;
};

template <typename Iter> inline
message::message(Iter begin, Iter end) :
		content_(begin, end) {
}

} // namespace

#endif // XIVA_MESSAGE_HPP_INCLUDED
