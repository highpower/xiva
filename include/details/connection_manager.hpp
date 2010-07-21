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
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/intrusive_ptr.hpp>

#define BOOST_MULTI_INDEX_DISABLE_SERIALIZATION

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/ordered_index.hpp>

#include "xiva/forward.hpp"
#include "xiva/globals.hpp"
#include "xiva/error.hpp"
#include "xiva/logger.hpp"
#include "xiva/connection_listener.hpp"

#include "details/connection.hpp"
#include "details/compound_listener.hpp"
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
	typedef boost::intrusive_ptr<connection_listener> listener_ptr_type;

	void clear();
	bool empty() const;
	void insert_connection(connection_ptr_type const &conn);
	void remove_connection(connection_ptr_type const &conn);

	void send(std::string const &to, boost::shared_ptr<message> const &m);
	void send(globals::connection_id const &to, boost::shared_ptr<message> const &m);

	void finish();
	void wait_for_complete();
	void init(settings const &s);
	void add_connection_listener(listener_ptr_type const &l);
	void attach_logger(boost::intrusive_ptr<logger> const &log);

private:
	bool finished() const;
	void fire_connection_opened(ConnectionBase const &conn);
	void fire_connection_closed(ConnectionBase const &conn);

	typedef std::allocator<connection_ptr_type> allocator_type;
	typedef cm_connection_id<ConnectionBase> conn_id_type;
	typedef mi::hashed_unique<conn_id_type> id_index_type;
	typedef cm_connection_name<ConnectionBase> conn_name_type;
	typedef mi::ordered_non_unique<conn_name_type> name_index_type;
	typedef mi::indexed_by<id_index_type, name_index_type> index_type;
	typedef mi::multi_index_container<connection_ptr_type, index_type, allocator_type> connection_set_type;

private:
	bool finished_;
	connection_set_type connections_;
	boost::intrusive_ptr<logger> logger_;
	boost::intrusive_ptr<compound_listener> listener_;
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
	finished_(false), listener_(l)
{
}

template <typename ConnectionBase>
connection_manager<ConnectionBase>::~connection_manager() {
}

template <typename ConnectionBase> void
connection_manager<ConnectionBase>::clear() {
	connections_.clear();
}

template <typename ConnectionBase> bool
connection_manager<ConnectionBase>::empty() const {
	return connections_.empty();
}

template <typename ConnectionBase> void
connection_manager<ConnectionBase>::insert_connection(connection_ptr_type const &conn) {
	if (finished()) {
		return;
	}
	assert(conn);
	std::pair<typename connection_set_type::iterator, bool> p = connections_.insert(conn);
	if (p.second) {
		fire_connection_opened(*conn);
	}
	else {
		throw error("same connection");
	}
}

template <typename ConnectionBase> void
connection_manager<ConnectionBase>::remove_connection(connection_ptr_type const &conn) {

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
	typedef typename connection_set_type::template nth_index<1>::type index_type;
	index_type &index = connections_.template get<1>();
	std::pair<typename index_type::iterator, typename index_type::iterator> p = index.equal_range(to);
	if (p.first == p.second) {
		listener_->disconnected(to);
		return;
	}
	std::vector<connection_ptr_type> conns;
	conns.reserve(std::distance(p.first, p.second));
	for ( ; p.first != p.second; ++p.first) {
		conns.push_back(*p.first);
	}
	for (typename std::vector<connection_ptr_type>::iterator it = conns.begin(), end = conns.end(); it != end; ++it) {
		connection_ptr_type &conn = *it;
		if (conn->send(m)) {
			logger_->debug("sending message to connection[%lu] by name %s", conn->id(), to.c_str());
		}
	}
}

template <typename ConnectionBase> void
connection_manager<ConnectionBase>::send(globals::connection_id const &to, boost::shared_ptr<message> const &m) {
	if (finished()) {
		return;
	}
	typedef typename connection_set_type::template nth_index<0>::type index_type;
	index_type &index = connections_.template get<0>();
	std::pair<typename index_type::iterator, typename index_type::iterator> p = index.equal_range(to);
	if (p.first == p.second) {
		return;
	}
	std::vector<connection_ptr_type> conns;
	conns.reserve(std::distance(p.first, p.second));
	for ( ; p.first != p.second; ++p.first) {
		conns.push_back(*p.first);
	}
	for (typename std::vector<connection_ptr_type>::iterator it = conns.begin(), end = conns.end(); it != end; ++it) {
		connection_ptr_type &conn = *it;
		if (conn->send(m)) {
			logger_->debug("sending message to connection[%lu] by id", conn->id());
		}
	}
}

template <typename ConnectionBase> void
connection_manager<ConnectionBase>::finish() {
	for (typename connection_set_type::iterator i = connections_.begin(), end = connections_.end(); i != end; ++i) {
		(*i)->finish();
	}
	finished_ = true;
}

template <typename ConnectionBase> void
connection_manager<ConnectionBase>::wait_for_complete() {
}

template <typename ConnectionBase> void
connection_manager<ConnectionBase>::init(settings const &s) {
	listener_->init(s);
}

template <typename ConnectionBase> void
connection_manager<ConnectionBase>::add_connection_listener(listener_ptr_type const &l) {
	listener_->add_connection_listener(l);
}

template <typename ConnectionBase> void
connection_manager<ConnectionBase>::attach_logger(boost::intrusive_ptr<logger> const &log) {
	logger_ = log;
	listener_->attach_logger(log);
}

template <typename ConnectionBase> bool
connection_manager<ConnectionBase>::finished() const {
	return finished_;
}

template <typename ConnectionBase> void
connection_manager<ConnectionBase>::fire_connection_opened(ConnectionBase const &conn) {
	listener_->connection_opened(conn.name(), conn.id());
}

template <typename ConnectionBase> void
connection_manager<ConnectionBase>::fire_connection_closed(ConnectionBase const &conn) {
	listener_->connection_closed(conn.name(), conn.id());
}

}
} // namespaces

#endif // XIVA_DETAILS_CONNECTION_MANAGER_HPP_INCLUDED
