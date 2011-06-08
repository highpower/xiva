#include "acsetup.hpp"
#include "details/message_queue.hpp"

#include <cassert>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/current_function.hpp>
#include <boost/lexical_cast.hpp>

#include "xiva/logger.hpp"
#include "xiva/message.hpp"
#include "details/connection_manager_base.hpp"

namespace xiva { namespace details {

message_queue::message_queue(asio::io_service::strand &st, boost::intrusive_ptr<connection_manager_base> const &cm) :
	strand_(st), accepting_messages_(true), manager_(cm)
{
	assert(manager_);
}

message_queue::~message_queue() {
}

void
message_queue::pop_by_name() {
	try {
		std::deque<queue_item_name_type> l;
		boost::mutex::scoped_lock sl(mutex_);
		l.swap(messages_by_name_);
		sl.unlock();

		for (std::deque<queue_item_name_type>::iterator i = l.begin(), end = l.end(); i != end; ++i) {
			std::string const &to = i->first;
			manager_->send(to, i->second);
			//logger_->debug("message sent to %s", to.c_str());
		}
	}
	catch (std::exception const &e) {
		logger_->error("exception was caught in %s: %s", BOOST_CURRENT_FUNCTION, e.what());
	}
}

void
message_queue::pop_by_id() {
	try {
		std::deque<queue_item_id_type> l;
		boost::mutex::scoped_lock sl(mutex_);
		l.swap(messages_by_id_);
		sl.unlock();

		for (std::deque<queue_item_id_type>::iterator i = l.begin(), end = l.end(); i != end; ++i) {
			globals::connection_id const &to = i->first;
			manager_->send(to, i->second);
			//logger_->debug("message sent to %s", boost::lexical_cast<std::string>(to).c_str());
		}
	}
	catch (std::exception const &e) {
		logger_->error("exception was caught in %s: %s", BOOST_CURRENT_FUNCTION, e.what());
	}
}

void
message_queue::finish() {
	boost::mutex::scoped_lock sl(mutex_);
	accepting_messages_ = false;
}

bool
message_queue::active() const {
	boost::mutex::scoped_lock sl(mutex_);
	return accepting_messages_;
}

void
message_queue::send(std::string const &to, boost::shared_ptr<message> const &m) {
	if (push_message(to, m)) {
		strand_.post(boost::bind(&message_queue::pop_by_name, this));
	}
}

void
message_queue::send(globals::connection_id to, boost::shared_ptr<message> const &m) {
	if (push_message(to, m)) {
		strand_.post(boost::bind(&message_queue::pop_by_id, this));
	}
}

void
message_queue::attach_logger(boost::intrusive_ptr<logger> const &log) {
	assert(log);
	logger_ = log;
}

bool
message_queue::push_message(std::string const &to, boost::shared_ptr<message> const &m) {
	boost::mutex::scoped_lock sl(mutex_);
	if (accepting_messages_) {
		messages_by_name_.push_back(std::make_pair(to, m));
	}
	return accepting_messages_;
}

bool
message_queue::push_message(globals::connection_id to, boost::shared_ptr<message> const &m) {
	boost::mutex::scoped_lock sl(mutex_);
	if (accepting_messages_) {
		messages_by_id_.push_back(std::make_pair(to, m));
	}
	return accepting_messages_;
}

}} // namespaces
