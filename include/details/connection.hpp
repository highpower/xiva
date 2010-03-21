/** @file connection.hpp */
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

#ifndef XIVA_DETAILS_CONNECTION_HPP_INCLUDED
#define XIVA_DETAILS_CONNECTION_HPP_INCLUDED

#include <string>
#include <boost/intrusive_ptr.hpp>

#include "xiva/forward.hpp"
#include "xiva/reference_counted.hpp"
#include "details/connection_base.hpp"

namespace xiva { namespace details {

class connection : public reference_counted, public connection_base {

public:
	connection();
	virtual ~connection();

	std::string const& name() const;
	std::string const& nameref() const;
	void name(std::string const &name);

private:
	connection(connection const &);
	connection& operator = (connection const &);

private:
	std::string name_;
};


inline std::string const&
connection::name() const {
	return name_;
}
	
inline std::string const&
connection::nameref() const {
	return name_;
}

inline void
connection::name(std::string const &name) {
	name_.assign(name);
}


}} // namespaces

#endif // XIVA_DETAILS_CONNECTION_HPP_INCLUDED
