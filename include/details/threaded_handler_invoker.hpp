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

#ifndef XIVA_DETAILS_THREADED_HANDLER_INVOKER_HPP_INCLUDED
#define XIVA_DETAILS_THREADED_HANDLER_INVOKER_HPP_INCLUDED

#include <string>

#include <boost/intrusive_ptr.hpp>
#include <boost/thread/thread.hpp>

#include "xiva/shared.hpp"
#include "xiva/forward.hpp"

#include "details/asio.hpp"
#include "details/threaded_queue.hpp"
#include "details/handler_invoker_base.hpp"

namespace xiva { namespace details {

class request_impl;
class request_holder;
class connection_data;
class threaded_connection;

class threaded_handler_invoker : public handler_invoker_base, private boost::thread_group {

public:
	threaded_handler_invoker(asio::io_service::strand &st, connection_data const &data);
	virtual ~threaded_handler_invoker();

	typedef threaded_connection connection_type;
	typedef boost::intrusive_ptr<connection_type> connection_ptr_type;

	void thread_func();

	virtual void finish();
	virtual void wait_for_complete();
	virtual void init(settings const &s);
	virtual void attach_logger(boost::intrusive_ptr<logger> const &log);
	void invoke_handler(connection_ptr_type conn, request_impl &req, response_impl &impl);

private:
	threaded_handler_invoker(threaded_handler_invoker const &);
	threaded_handler_invoker& operator = (threaded_handler_invoker const &);
	
	typedef boost::intrusive_ptr<request_holder> holder_ptr_type;
	typedef std::pair<holder_ptr_type, connection_ptr_type> item_type;
	typedef threaded_queue<item_type> queue_type;
	typedef queue_type::raw_items_type items_type;

	static void handle_item(item_type &item);

private:
	asio::io_service::strand &strand_;
	boost::intrusive_ptr<logger> logger_;
	boost::intrusive_ptr<response_handler> handler_;

	queue_type input_queue_;
};

}} // namespaces

#endif // XIVA_DETAILS_THREADED_HANDLER_INVOKER_HPP_INCLUDED
