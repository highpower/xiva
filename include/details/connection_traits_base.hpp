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

#ifndef XIVA_DETAILS_CONNECTION_TRAITS_BASE_HPP_INCLUDED
#define XIVA_DETAILS_CONNECTION_TRAITS_BASE_HPP_INCLUDED

#include <boost/intrusive_ptr.hpp>

#include "xiva/shared.hpp"
#include "xiva/forward.hpp"

namespace xiva { namespace details {

class connection_traits_base : public shared {

public:
	connection_traits_base();
	virtual ~connection_traits_base();

	virtual void finish() = 0;
	virtual void init(settings const &s) = 0;
	virtual void attach_logger(boost::intrusive_ptr<logger> const &log) = 0;

private:
	connection_traits_base(connection_traits_base const &);
	connection_traits_base& operator = (connection_traits_base const &);
};

}} // namespaces

#endif // XIVA_DETAILS_CONNECTION_TRAITS_BASE_HPP_INCLUDED
