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

#ifndef XIVA_DETAILS_SERVER_IMPL_HPP_INCLUDED
#define XIVA_DETAILS_SERVER_IMPL_HPP_INCLUDED

#include <vector>
#include <string>
#include <deque>

#include <boost/function.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/condition.hpp>

#include "xiva/config.hpp"
#include "xiva/forward.hpp"
#include "xiva/component_set.hpp"
#include "xiva/globals.hpp"

#include "details/asio.hpp"

namespace xiva { namespace details {

class acceptor_base;
class connection_data;
class message_queue;
class threaded_listener;
class connection_traits_base;
class connection_manager_base;
class handler_invoker_base;

class server_impl : public component_set, private boost::thread_group {

public:
	server_impl();
	virtual ~server_impl();

	typedef boost::function<globals::provider_type> provider_closure_type;

	void stop();
	void init(settings const &s);
	boost::shared_ptr<channels_stat_impl> const& init_channels_stat();
	void start();

	void send(std::string const &to, boost::shared_ptr<message> const &m);
	void send(globals::connection_id to, boost::shared_ptr<message> const &m);

	void notify_connection_opened_failed(std::string const &to, globals::connection_id id);

	virtual void attach_logger(boost::intrusive_ptr<logger> const &logger);
	virtual void attach_message_filter(boost::intrusive_ptr<message_filter> const &filter);
	virtual void attach_response_handler(boost::intrusive_ptr<response_handler> const &m);
	virtual void attach_ping_formatter(boost::intrusive_ptr<ping_formatter> const &f);
	virtual void attach_formatter_creator(std::string const &fmt_id, boost::intrusive_ptr<formatter_creator> const &creator);
	virtual void add_connection_listener(boost::intrusive_ptr<connection_listener> const &l);
	virtual void start_provider(unsigned short nthreads, boost::function<globals::provider_type> f);

private:
	server_impl(server_impl const &);
	server_impl& operator = (server_impl const &);

	typedef std::pair<provider_closure_type, unsigned short> thread_param_type;

	void start_provider_thread(thread_param_type const &tp);
	void provider_thread_func(boost::function<globals::provider_type> f);

	void run_io();
	void finish();

	template <typename invoker_type>
	void init_data(settings const &s, bool ssl);

private:
	asio::io_service io_;
	asio::io_service::strand strand_;

	boost::scoped_ptr<connection_data> data_;
	std::vector<thread_param_type> providers_;

	boost::mutex stop_mutex_;
	boost::condition stop_condition_;

	boost::intrusive_ptr<logger> logger_;
	boost::intrusive_ptr<ping_formatter> ping_formatter_;
	boost::intrusive_ptr<message_filter> message_filter_;
	boost::intrusive_ptr<connection_manager_base> connection_manager_;
	boost::intrusive_ptr<response_handler> handler_;
	boost::intrusive_ptr<threaded_listener> listener_;
	boost::intrusive_ptr<connection_traits_base> ssl_connection_traits_;
	boost::intrusive_ptr<connection_traits_base> connection_traits_;
	boost::intrusive_ptr<handler_invoker_base> handler_invoker_;
	boost::intrusive_ptr<acceptor_base> ssl_acceptor_;
	boost::intrusive_ptr<acceptor_base> acceptor_;
	boost::intrusive_ptr<message_queue> message_queue_;
	bool started_;
	bool stopped_;
};


}} // namespaces

#endif // XIVA_DETAILS_SERVER_IMPL_HPP_INCLUDED
