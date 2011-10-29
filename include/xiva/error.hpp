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

#ifndef XIVA_ERROR_HPP_INCLUDED
#define XIVA_ERROR_HPP_INCLUDED

#include <exception>

#include "xiva/config.hpp"

namespace xiva {

/**
 * This is the xiva-specific exception.
 */
class XIVA_API error : public std::exception {

public:
	/** */
	error(char const *format, ...);

	/** destructor */
	virtual ~error() throw ();

	/** inherited from std::exception
	 * @return textual description of error.
	 */
	virtual char const* what() const throw ();

	enum { message_size = 128 };

private:
	char buffer_[message_size];
};

} // namespace

#endif // XIVA_ERROR_HPP_INCLUDED
