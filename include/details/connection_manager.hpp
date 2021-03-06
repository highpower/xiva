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

#ifndef XIVA_DETAILS_CONNECTION_MANAGER_HPP_INCLUDED
#define XIVA_DETAILS_CONNECTION_MANAGER_HPP_INCLUDED

#include <cassert>
#include <iterator>
#include <utility>
#include <string>
#include <list>
#include <boost/shared_ptr.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

#define BOOST_MULTI_INDEX_DISABLE_SERIALIZATION

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/ordered_index.hpp>

#include "xiva/forward.hpp"
#include "xiva/globals.hpp"
#include "xiva/error.hpp"

#include "details/connection_base.hpp"
#include "details/connection_manager_base.hpp"

namespace mi = boost::multi_index;

namespace xiva { namespace details {

template <typename ConnectionBase>
struct cm_connection_id {
	typedef globals::connection_id result_type;
	typedef boost::intrusive_ptr<ConnectionBase> connection_ptr_type;
	result_type operator () (connection_ptr_type const &conn) const;
};

template <typename ConnectionBase>
struct cm_connection_name {
	typedef std::string result_type;
	typedef boost::intrusive_ptr<ConnectionBase> connection_ptr_type;
	result_type const& operator () (connection_ptr_type const &conn) const;
};


template <typename ConnectionBase>
class connection_manager : public connection_manager_base {

public:
	connection_manager(boost::intrusive_ptr<compound_listener> const &l);
	virtual ~connection_manager();

	typedef typename boost::intrusive_ptr<ConnectionBase> connection_ptr_type;

	bool empty() const; // for test only
	bool insert_connection(connection_ptr_type const &conn); // from stranded thread
	void remove_connection(connection_ptr_type const &conn); // from stranded thread

	virtual void send(std::string const &to, boost::shared_ptr<message> const &m); // from stranded thread
	virtual void send(globals::connection_id const &to, boost::shared_ptr<message> const &m); // from stranded thread

	virtual void notify_connection_opened_failed(std::string const &to, globals::connection_id id); // from stranded thread

	virtual void finish();            // from stranded thread
	virtual void wait_for_complete(); // from non-stranded thread

private:
	bool finished() const;

	typedef std::allocator<connection_ptr_type> allocator_type;
	typedef cm_connection_id<ConnectionBase> conn_id_type;
	typedef mi::hashed_unique<conn_id_type> id_index_type;
	typedef cm_connection_name<ConnectionBase> conn_name_type;
	typedef mi::ordered_non_unique<conn_name_type> name_index_type;
	typedef mi::indexed_by<id_index_type, name_index_type> index_type;
	typedef mi::multi_index_container<connection_ptr_type, index_type, allocator_type> connection_set_type;

private:
	connection_set_type connections_;
	boost::mutex mutex_;
	boost::condition condition_;
	bool finished_;
};


template <typename ConnectionBase> globals::connection_id
cm_connection_id<ConnectionBase>::operator () (connection_ptr_type const &conn) const {
	assert(conn);
	return conn->id();
}

template <typename ConnectionBase> std::string const&
cm_connection_name<ConnectionBase>::operator () (connection_ptr_type const &conn) const {
	assert(conn);
	return conn->name();
}


template <typename ConnectionBase>
connection_manager<ConnectionBase>::connection_manager(boost::intrusive_ptr<compound_listener> const &l) :
	connection_manager_base(l), finished_(false)
{
}

template <typename ConnectionBase>
connection_manager<ConnectionBase>::~connection_manager() {
}

template <typename ConnectionBase> bool
connection_manager<ConnectionBase>::empty() const {
	return connections_.empty();
}

template <typename ConnectionBase> bool
connection_manager<ConnectionBase>::insert_connection(connection_ptr_type const &conn) {

	if (finished()) {
		return false; // don't try remove_connection() in the future
	}
	assert(conn);

	std::pair<typename connection_set_type::iterator, bool> p = connections_.insert(conn);
	if (!p.second) {
		throw error("same connection");
	}
	fire_connection_opened(*conn);
	return true;
}

template <typename ConnectionBase> void
connection_manager<ConnectionBase>::remove_connection(connection_ptr_type const &conn) {

	if (finished()) {
		// the listener already finished
		// the container will be clear in finish()
		return;
	}
	assert(conn);

	typedef typename connection_set_type::template nth_index<0>::type index_type;

	index_type &index = connections_.template get<0>();
	index.erase(conn->id());
	fire_connection_closed(*conn);
}

template <typename ConnectionBase> void
connection_manager<ConnectionBase>::send(std::string const &to, boost::shared_ptr<message> const &m) {
	if (finished()) {
		return;
	}
	assert(m);
	typedef typename connection_set_type::template nth_index<1>::type index_type;
	index_type &index = connections_.template get<1>();
	std::pair<typename index_type::iterator, typename index_type::iterator> p = index.equal_range(to);
	if (p.first == p.second) {
		fire_disconnected(to);
		return;
	}
	message_filter const *filter = msg_filter();
	std::list<connection_ptr_type> conns;
	for ( ; p.first != p.second; ++p.first) {
		connection_ptr_type const &conn = *p.first;
		if (conn->allow_message(*m, filter)) {
			conns.push_back(conn);
		}
	}
	for (typename std::list<connection_ptr_type>::iterator it = conns.begin(), end = conns.end(); it != end; ++it) {
		connection_ptr_type &conn = *it;
		conn->send(m);
	}
}

template <typename ConnectionBase> void
connection_manager<ConnectionBase>::send(globals::connection_id const &to, boost::shared_ptr<message> const &m) {
	if (finished()) {
		return;
	}
	assert(m);
	typedef typename connection_set_type::template nth_index<0>::type index_type;
	index_type &index = connections_.template get<0>();
	std::pair<typename index_type::iterator, typename index_type::iterator> p = index.equal_range(to);
	if (p.first == p.second) {
		return;
	}
	message_filter const *filter = msg_filter();
	std::list<connection_ptr_type> conns;
	for ( ; p.first != p.second; ++p.first) {
		connection_ptr_type const &conn = *p.first;
		if (conn->allow_message(*m, filter)) {
			conns.push_back(conn);
		}
	}
	for (typename std::list<connection_ptr_type>::iterator it = conns.begin(), end = conns.end(); it != end; ++it) {
		connection_ptr_type &conn = *it;
		conn->send(m);
	}
}

template <typename ConnectionBase> void
connection_manager<ConnectionBase>::notify_connection_opened_failed(std::string const &to, globals::connection_id id) {

	(void) to;

	if (finished()) {
		return;
	}
	typedef typename connection_set_type::template nth_index<0>::type index_type;
	index_type &index = connections_.template get<0>();
	std::pair<typename index_type::iterator, typename index_type::iterator> p = index.equal_range(id);
	if (p.first != p.second) {
		connection_ptr_type conn = *p.first;
		if (conn->name() == to) {
			conn->close();
		}
	}
}

template <typename ConnectionBase> void
connection_manager<ConnectionBase>::finish() {

	std::list<connection_ptr_type> conns;
	{
		boost::mutex::scoped_lock lock(mutex_);
		if (finished_) {
			return;
		}
		finished_ = true;

		for (typename connection_set_type::iterator i = connections_.begin(), end = connections_.end(); i != end; ++i) {
			conns.push_back(*i);
		}
		connections_.clear();
	}

	for (typename std::list<connection_ptr_type>::iterator it = conns.begin(), end = conns.end(); it != end; ++it) {
		connection_ptr_type &conn = *it;
		try {
			conn->finish();
			conn->close();
		}
		catch (...) {
		}
	}
	condition_.notify_all();
}

template <typename ConnectionBase> void
connection_manager<ConnectionBase>::wait_for_complete() {
	boost::mutex::scoped_lock lock(mutex_);
	while (!finished_) {
		condition_.wait(lock);
	}
}

template <typename ConnectionBase> bool
connection_manager<ConnectionBase>::finished() const {
	return finished_;
}

}} // namespaces

#endif // XIVA_DETAILS_CONNECTION_MANAGER_HPP_INCLUDED
