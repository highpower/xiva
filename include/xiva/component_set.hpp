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

class XIVA_API component_set {

public:
	component_set();
	virtual ~component_set();

	virtual void attach_logger(boost::intrusive_ptr<logger> const &log) = 0;
	virtual void attach_receiver_matcher(boost::intrusive_ptr<receiver_matcher> const &m) = 0;
	virtual void add_connection_listener(boost::intrusive_ptr<connection_listener> const &l) = 0;
	virtual void start_provider(unsigned short nthreads, boost::function<globals::provider_type> f) = 0;

private:
	component_set(component_set const &);
	component_set& operator = (component_set const &);
};

} // namespace

#endif // XIVA_COMPONENT_SET_HPP_INCLUDED
