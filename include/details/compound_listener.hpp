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

#ifndef XIVA_DETAILS_COMPOUND_LISTENER_HPP_INCLUDED
#define XIVA_DETAILS_COMPOUND_LISTENER_HPP_INCLUDED

#include <list>
#include <string>
#include <boost/intrusive_ptr.hpp>

#include "xiva/forward.hpp"
#include "xiva/globals.hpp"
#include "xiva/connection_listener.hpp"

namespace xiva { namespace details {

class compound_listener : public connection_listener {

public:
	compound_listener();
	virtual ~compound_listener();

	typedef boost::intrusive_ptr<connection_listener> listener_ptr_type;

	virtual void connection_opened(std::string const &to) throw (std::exception);
	virtual void connection_closed(std::string const &to) throw (std::exception);

	virtual void init(settings const &s);
	virtual void add_connection_listener(listener_ptr_type const &l);

	void attach_logger(boost::intrusive_ptr<logger> const &log);

protected:
	boost::intrusive_ptr<logger> const& log() const;
	void notify_connection_opened(std::string const &to);
	void notify_connection_closed(std::string const &to);

private:
	compound_listener(compound_listener const &);
	compound_listener& operator = (compound_listener const &);

private:
	boost::intrusive_ptr<logger> logger_;
	std::list<listener_ptr_type> listeners_;
};

}} // namespaces

#endif // XIVA_DETAILS_COMPOUND_LISTENER_HPP_INCLUDED
