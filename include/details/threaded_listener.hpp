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

#ifndef XIVA_DETAILS_THREADED_LISTENER_HPP_INCLUDED
#define XIVA_DETAILS_THREADED_LISTENER_HPP_INCLUDED

#include <list>
#include <string>
#include <utility>
#include <boost/thread/thread.hpp>

#include "details/threaded_queue.hpp"
#include "details/compound_listener.hpp"

namespace xiva { namespace details {

class threaded_listener : public compound_listener, private boost::thread_group {

public:
	threaded_listener();
	virtual ~threaded_listener();

	void thread_func();
	virtual void init(settings const &s);

	void finish();

	virtual void connection_opened(std::string const &to, globals::connection_id id) throw (std::exception);
	virtual void connection_closed(std::string const &to, globals::connection_id id) throw (std::exception);

private:
	threaded_listener(threaded_listener const &);
	threaded_listener& operator = (threaded_listener const &);

	typedef std::pair<std::string, globals::connection_id> data_type;
	typedef std::pair<data_type, bool> queue_item_type;

private:
	threaded_queue<queue_item_type> items_;
};

}} // namespaces

#endif // XIVA_DETAILS_THREADED_LISTENER_HPP_INCLUDED
