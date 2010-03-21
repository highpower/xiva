/** @file threaded_connection_validator.hpp */
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

#ifndef XIVA_DETAILS_THREADED_CONNECTION_VALIDATOR_HPP_INCLUDED
#define XIVA_DETAILS_THREADED_CONNECTION_VALIDATOR_HPP_INCLUDED

#include <string>
#include <exception>

#include <boost/intrusive_ptr.hpp>
#include <boost/thread/thread.hpp>


#include "xiva/forward.hpp"
#include "xiva/receiver_matcher.hpp"
#include "xiva/reference_counted.hpp"
#include "xiva/logger.hpp"

#include "details/asio.hpp"
#include "details/threaded_connection.hpp"
#include "details/threaded_queue.hpp"
#include "details/request_impl.hpp"


namespace xiva { namespace details {

class connection_data;


class threaded_connection_validator : public reference_counted, private boost::thread_group {

public:
	threaded_connection_validator(asio::io_service &io, connection_data const &data);
	virtual ~threaded_connection_validator();

	void thread_func();
	void pop();

	typedef threaded_connection connection_type;
	typedef boost::intrusive_ptr<connection_type> connection_type_ptr;

	bool validate(connection_type_ptr conn, request_impl &req);
	void attach_logger(boost::intrusive_ptr<logger> const &log);
	void init(settings const &s);

private:
	struct request_provider : public reference_counted {
		request_impl req;
	};
	typedef boost::intrusive_ptr<request_provider> request_provider_ptr;

	typedef std::pair<request_provider_ptr, connection_type_ptr> queue_item_type;

private:
	asio::io_service &io_;
	asio::io_service::strand strand_;
	boost::intrusive_ptr<receiver_matcher> matcher_;
	boost::intrusive_ptr<logger> logger_;

	threaded_queue<queue_item_type> input_queue_;
        std::deque<connection_type_ptr> validated_connections_;
        mutable boost::mutex mutex_;
};



}} // namespaces

#endif // XIVA_DETAILS_THREADED_CONNECTION_VALIDATOR_HPP_INCLUDED
