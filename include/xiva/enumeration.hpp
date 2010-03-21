/** @file enumeration.hpp */
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

#ifndef XIVA_ENUMERATION_HPP_INCLUDED
#define XIVA_ENUMERATION_HPP_INCLUDED

#include <boost/intrusive_ptr.hpp>

#include "xiva/config.hpp"
#include "xiva/reference_counted.hpp"

namespace xiva {

template <typename Item>
class XIVA_API enumeration : public reference_counted {

public:
	enumeration();
	virtual ~enumeration();
	
	typedef Item value_type;
	typedef enumeration<value_type> type;
	typedef boost::intrusive_ptr<type> ptr_type;

	virtual Item next() const = 0;
	virtual bool empty() const = 0;

private:
	enumeration(enumeration const &);
	enumeration& operator = (enumeration const &);
};

template <typename Item> inline
enumeration<Item>::enumeration()
{
}

template <typename Item> inline
enumeration<Item>::~enumeration() {
}

} // namespace

#endif // XIVA_ENUMERATION_HPP_INCLUDED
