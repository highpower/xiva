#include "acsetup.hpp"
#include "details/threaded_matcher_invoker.hpp"

#include <string>
#include <exception>
#include <boost/bind.hpp>
#include <boost/current_function.hpp>
#include <boost/function.hpp>

#include "xiva/logger.hpp"
#include "xiva/settings.hpp"
#include "details/request_impl.hpp"
#include "details/connection_data.hpp"
#include "details/threaded_shared.hpp"
#include "details/threaded_connection.hpp"

namespace xiva { namespace details {

struct request_holder : public threaded_shared {
	request_impl req;
};

threaded_matcher_invoker::threaded_matcher_invoker(asio::io_service &io, connection_data const &data) : 
	io_(io), strand_(io)
{
	boost::intrusive_ptr<receiver_matcher> matcher = data.matcher();
	assert(matcher);
	assert(matcher->threaded());
	matcher_ = matcher;
}

threaded_matcher_invoker::~threaded_matcher_invoker() {
	try {
		input_queue_.finish();
		join_all();
	}
	catch (std::exception const &e) {
		(void) e;
	}
}

void
threaded_matcher_invoker::init(settings const &s) {
	assert(logger_);
	boost::function<void()> f = boost::bind(&threaded_matcher_invoker::thread_func, this);
	unsigned short threads = s.matcher_threads();
	for (unsigned short i = 0; i < threads; ++i) {
		create_thread(f);
	}
}

void
threaded_matcher_invoker::attach_logger(boost::intrusive_ptr<logger> const &log) {
	logger_ = log;
}

void
threaded_matcher_invoker::invoke_matcher(threaded_matcher_invoker::connection_ptr_type conn, request_impl &req) {
	matcher_->check(req);
	holder_ptr_type holder(new request_holder());
	holder->req.swap(req);
	input_queue_.push(queue_item_type(holder, conn));
}

void
threaded_matcher_invoker::pop() {
	std::deque<connection_ptr_type> d;
	pop(d);
	try {
		char const *content_type = matcher_->content_type();
		for (std::deque<connection_ptr_type>::iterator i = d.begin(), end = d.end(); i != end; ++i) {
			(*i)->matched((*i)->nameref(), content_type); // wtf??? we have already recognized connection name 
		}
	}
	catch (std::exception const &e) {
		logger_->error("exception was caught in %s: %s", BOOST_CURRENT_FUNCTION, e.what());
	}
}

void
threaded_matcher_invoker::thread_func() {
	queue_item_type item;
	while (input_queue_.pop(item)) {
		try {
			std::string receiver = matcher_->receiver(item.first->req);
			item.second->name(receiver);
			push_matched(item.second);
			strand_.dispatch(boost::bind(&threaded_matcher_invoker::pop, this));
		}
		catch (std::exception const &e) {
			logger_->error("exception was caught in %s: %s", BOOST_CURRENT_FUNCTION, e.what());
		}
	}
}

void
threaded_matcher_invoker::push_matched(threaded_matcher_invoker::connection_ptr_type conn) {
	boost::mutex::scoped_lock sl(mutex_);
	connections_.push_back(conn);
}

void
threaded_matcher_invoker::pop(std::deque<threaded_matcher_invoker::connection_ptr_type> &deque) {
	deque.clear();
	boost::mutex::scoped_lock sl(mutex_);
	deque.swap(connections_);
}

}} // namespaces
