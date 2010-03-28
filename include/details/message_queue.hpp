/** @file message_queue.hpp */
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

#ifndef XIVA_DETAILS_MESSAGE_QUEUE_HPP_INCLUDED
#define XIVA_DETAILS_MESSAGE_QUEUE_HPP_INCLUDED

#include <deque>
#include <string>
#include <utility>

#include <boost/shared_ptr.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/thread/mutex.hpp>

#include "xiva/shared.hpp"
#include "xiva/forward.hpp"
#include "xiva/globals.hpp"
#include "xiva/message_sender.hpp"
#include "details/asio.hpp"

namespace xiva { namespace details {

class connection_manager_base;

class message_queue : public message_sender, public shared {

public:
	message_queue(asio::io_service &io, boost::intrusive_ptr<connection_manager_base> const &cm);
	virtual ~message_queue();

	void pop_by_name();
	void pop_by_id();
	void finish();
	void attach_logger(boost::intrusive_ptr<logger> const &log);

	virtual bool active() const;
	virtual void send(std::string const &to, boost::shared_ptr<message> const &m);
	virtual void send(globals::connection_id const &id, boost::shared_ptr<message> const &m);

private:
	message_queue(message_queue const &);
	message_queue& operator = (message_queue const &);

	typedef boost::shared_ptr<message> message_ptr_type;
	typedef std::pair<std::string, message_ptr_type> queue_item_name_type;
	typedef std::pair<globals::connection_id, message_ptr_type> queue_item_id_type;

	void push_message(std::string const &to, boost::shared_ptr<message> const &m);
	void push_message(globals::connection_id const &to, boost::shared_ptr<message> const &m);

private:
	asio::io_service &io_;
	asio::io_service::strand strand_;
	std::deque<queue_item_name_type> messages_by_name_;
	std::deque<queue_item_id_type> messages_by_id_;

	bool accepting_messages_;
	boost::intrusive_ptr<logger> logger_;
	boost::intrusive_ptr<connection_manager_base> manager_;
	mutable boost::mutex mutex_;
};

}} // namespaces

#endif // XIVA_DETAILS_MESSAGE_QUEUE_HPP_INCLUDED
