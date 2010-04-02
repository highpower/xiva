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

#ifndef XIVA_CONNECTION_LISTENER_HPP_INCLUDED
#define XIVA_CONNECTION_LISTENER_HPP_INCLUDED

#include <string>
#include <exception>

#include "xiva/config.hpp"
#include "xiva/globals.hpp"
#include "xiva/shared.hpp"

namespace xiva {

/** 
 * This interface provides two callbacks which are called when 
 * connection to a particular user is established or closed.
 * You shoud implement this interface and provide an instance to 
 * the system to know whether to send messages to the particular 
 * user or not.
 */
class XIVA_API connection_listener : public shared {

public:
	/** default constructor */
	connection_listener();
	
	/** destructor */
	virtual ~connection_listener();

	/** this function is called when the connection to the user is 
	 * established and the user or subsystem that initiated the request 
	 * is matched
	 * @param to user or subsystem name
	 * @param id the identifier of connection
	 */
	virtual void connection_opened(std::string const &to, globals::connection_id id) throw (std::exception) = 0;
	
	/** this function is called when the connection to the user is closed
	 * @param to user or subsystem name
	 * @param id the identifier of connection
	 */
	virtual void connection_closed(std::string const &to, globals::connection_id id) throw (std::exception) = 0;

private:
	/** should never be implemented */
	connection_listener(connection_listener const &);
	
	/** should never be implemented */
	connection_listener& operator = (connection_listener const &);
};

} // namespace

#endif // XIVA_CONNECTION_LISTENER_HPP_INCLUDED
