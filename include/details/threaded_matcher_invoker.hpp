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

#ifndef XIVA_DETAILS_THREADED_MATCHER_INVOKER_HPP_INCLUDED
#define XIVA_DETAILS_THREADED_MATCHER_INVOKER_HPP_INCLUDED

#include <string>
#include <exception>

#include <boost/intrusive_ptr.hpp>
#include <boost/thread/thread.hpp>

#include "xiva/shared.hpp"
#include "xiva/logger.hpp"
#include "xiva/forward.hpp"
#include "xiva/receiver_matcher.hpp"

#include "details/asio.hpp"
#include "details/threaded_queue.hpp"

namespace xiva { namespace details {

class request_impl;
class request_holder;
class connection_data;
class threaded_connection;

class threaded_matcher_invoker : public shared, private boost::thread_group {

public:
	threaded_matcher_invoker(asio::io_service &io, connection_data const &data);
	virtual ~threaded_matcher_invoker();

	typedef threaded_connection connection_type;
	typedef boost::intrusive_ptr<connection_type> connection_ptr_type;

	void pop();
	void thread_func();

	void init(settings const &s);
	void attach_logger(boost::intrusive_ptr<logger> const &log);
	void invoke_matcher(connection_ptr_type conn, request_impl &req);

private:
	threaded_matcher_invoker(threaded_matcher_invoker const &);
	threaded_matcher_invoker& operator = (threaded_matcher_invoker const &);
	
	typedef boost::intrusive_ptr<request_holder> holder_ptr_type;
	typedef std::pair<holder_ptr_type, connection_ptr_type> queue_item_type;
	
	void push_matched(connection_ptr_type conn);
	void pop(std::deque<connection_ptr_type> &deque);

private:
	asio::io_service &io_;
	asio::io_service::strand strand_;
	boost::intrusive_ptr<logger> logger_;
	boost::intrusive_ptr<receiver_matcher> matcher_;

	mutable boost::mutex mutex_;
	threaded_queue<queue_item_type> input_queue_;
	std::deque<connection_ptr_type> connections_;
};

}} // namespaces

#endif // XIVA_DETAILS_THREADED_MATCHER_INVOKER_HPP_INCLUDED
