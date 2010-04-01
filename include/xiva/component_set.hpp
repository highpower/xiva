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

#ifndef XIVA_COMPONENT_SET_HPP_INCLUDED
#define XIVA_COMPONENT_SET_HPP_INCLUDED

#include <boost/function.hpp>
#include <boost/intrusive_ptr.hpp>

#include "xiva/config.hpp"
#include "xiva/globals.hpp"
#include "xiva/forward.hpp"

namespace xiva {

/** 
 * This is an interface of container that holds references to 
 * common components such as logger. It helps to provide 
 * components from dynamically loaded modules to the system.
 */
class XIVA_API component_set {

public:
	/** default constructor */
	component_set();
	
	/** default destructor */
	virtual ~component_set();

	/** attaches an instance of the logger. 
	 * @param log an instance of class that implements logger interface.
	 */ 
	virtual void attach_logger(boost::intrusive_ptr<logger> const &log) = 0;
	
	/** sets up the receiver matcher 
	 * @param m an instance of class that implements receiver_matcher interface.
	 */
	virtual void attach_response_handler(boost::intrusive_ptr<response_handler> const &m) = 0;
	
	/** adds the connection_listener to the system.
	 * @param l an instance of class that implements connection_listener interface.
	 */
	virtual void add_connection_listener(boost::intrusive_ptr<connection_listener> const &l) = 0;

	/** starts some threads that program-specific message generator is executed by
	 * @param nthreads thread count
	 * @param f function that will be executed by the threads
	 */
	virtual void start_provider(unsigned short nthreads, boost::function<globals::provider_type> f) = 0;

private:
	/** shuold never be implemented */
	component_set(component_set const &);
	
	/** shuold never be implemented */
	component_set& operator = (component_set const &);
};

} // namespace

#endif // XIVA_COMPONENT_SET_HPP_INCLUDED
