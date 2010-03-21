/** @file threaded_connection.hpp */
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

#ifndef XIVA_DETAILS_THREADED_CONNECTION_HPP_INCLUDED
#define XIVA_DETAILS_THREADED_CONNECTION_HPP_INCLUDED

#include <string>
#include <boost/intrusive_ptr.hpp>

#include "xiva/forward.hpp"
#include "xiva/threaded_reference_counted.hpp"
#include "details/connection_base.hpp"

namespace xiva { namespace details {

class threaded_connection : public threaded_reference_counted, public connection_base {

public:
	threaded_connection();
	virtual ~threaded_connection();

	std::string name() const;
	std::string const& nameref() const;
	void name(std::string const &name);

private:
	threaded_connection(threaded_connection const &);
	threaded_connection& operator = (threaded_connection const &);

private:
	std::string name_;
};


inline std::string const&
threaded_connection::nameref() const {
	return name_;
}


}} // namespaces

#endif // XIVA_DETAILS_THREADED_CONNECTION_HPP_INCLUDED
