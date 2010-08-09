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

#ifndef XIVA_FORMATTER_CREATOR_HPP_INCLUDED
#define XIVA_FORMATTER_CREATOR_HPP_INCLUDED

#include <memory>

#include "xiva/config.hpp"
#include "xiva/shared.hpp"
#include "xiva/forward.hpp"

namespace xiva {

class XIVA_API formatter_creator : public shared {

public:
	formatter_creator();
	virtual ~formatter_creator();

	virtual std::auto_ptr<formatter> create(request const &req) const = 0;

private:
	formatter_creator(formatter_creator const &);
	formatter_creator& operator = (formatter_creator const &);
};

} // namespace

#endif // XIVA_FORMATTER_CREATOR_HPP_INCLUDED
