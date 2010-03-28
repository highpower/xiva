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

#ifndef XIVA_RECEIVER_MATCHER_HPP_INCLUDED
#define XIVA_RECEIVER_MATCHER_HPP_INCLUDED

#include <string>

#include "xiva/config.hpp"
#include "xiva/shared.hpp"

namespace xiva {

class request;
class response;

class XIVA_API receiver_matcher : public shared {

public:
	receiver_matcher();
	virtual ~receiver_matcher();

	virtual bool threaded() const = 0;
	virtual char const* content_type() const = 0;

	virtual void check(request const &req) const;
	virtual std::string receiver(request const &req) const = 0;

private:
	receiver_matcher(receiver_matcher const &);
	receiver_matcher& operator = (receiver_matcher const &);
};

} // namespace

#endif // XIVA_RECEIVER_MATCHER_HPP_INCLUDED
