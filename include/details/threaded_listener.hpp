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
#include <vector>
#include <string>
#include <utility>

#include <boost/thread/thread.hpp>
#include <boost/shared_ptr.hpp>

#include "details/threaded_queue.hpp"
#include "details/guard.hpp"
#include "details/compound_listener.hpp"

namespace xiva { namespace details {

class connection_data;

class threaded_listener : public compound_listener, private boost::thread_group {

public:
	threaded_listener(connection_data const &data);
	virtual ~threaded_listener();

	void thread_func(unsigned int index);
	virtual void init(settings const &s);

	void finish();

	virtual void connection_opened_ex(
		std::string const &to, globals::connection_id id,
		boost::shared_ptr<guard> const &notify_guard) throw (std::exception);
	virtual void connection_closed_ex(
		std::string const &to, globals::connection_id id,
		boost::shared_ptr<guard> const &notify_guard) throw (std::exception);

	virtual void connection_opened(std::string const &to, globals::connection_id id) throw (std::exception);
	virtual void connection_closed(std::string const &to, globals::connection_id id) throw (std::exception);
	virtual void disconnected(std::string const &to) throw (std::exception);

private:
	threaded_listener(threaded_listener const &);
	threaded_listener& operator = (threaded_listener const &);

	unsigned int calc_index(std::string const &to) const;

	struct queue_item_type {
		boost::shared_ptr<guard> notify_guard;
		std::string to;
		globals::connection_id id;
		bool for_open;
	};
	typedef threaded_queue<queue_item_type> queue_type;

private:
	connection_data const &data_;
	std::vector< boost::shared_ptr<queue_type> > items_;
};

}} // namespaces

#endif // XIVA_DETAILS_THREADED_LISTENER_HPP_INCLUDED
