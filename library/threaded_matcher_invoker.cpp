#include "acsetup.hpp"
#include "details/threaded_connection_validator.hpp"

#include <boost/bind.hpp>
#include <boost/current_function.hpp>


#include <string>
#include <exception>

#include "xiva/logger.hpp"
#include "xiva/settings.hpp"

#include "details/connection_data.hpp"


namespace xiva { namespace details {


threaded_connection_validator::threaded_connection_validator(asio::io_service &io, connection_data const &data) : io_(io), strand_(io)
{
	boost::intrusive_ptr<receiver_matcher> matcher = data.matcher();
	assert(matcher);
	assert(matcher->threaded());
	matcher_ = matcher;
}

threaded_connection_validator::~threaded_connection_validator()
{
	try {
		input_queue_.finish();
		join_all();
	}
	catch (std::exception const &e) {
	}
}
 
void threaded_connection_validator::thread_func() {
	queue_item_type item;
	while (input_queue_.pop(item)) {
		try {
			std::string receiver = matcher_->receiver(item.first->req);
			if (!receiver.empty()) {
				item.second->name(receiver);
			}
		}
		catch (std::exception const &e) {
			logger_->error("exception was caught in %s: %s", BOOST_CURRENT_FUNCTION, e.what());
		}
		boost::mutex::scoped_lock sl(mutex_);
		validated_connections_.push_back(item.second);
		sl.unlock();
	        strand_.dispatch(boost::bind(&threaded_connection_validator::pop, this));
	}
}

void threaded_connection_validator::pop() {
	try {
		std::deque<connection_type_ptr> l;
		boost::mutex::scoped_lock sl(mutex_);
		l.swap(validated_connections_);
		sl.unlock();
		char const *content_type = matcher_->content_type();
		for (std::deque<connection_type_ptr>::iterator i = l.begin(), end = l.end(); i != end; ++i) {
			connection_type_ptr &conn = *i;
			conn->validate_result(conn->nameref(), content_type);
		}
	}
	catch (std::exception const &e) {
		logger_->error("exception was caught in %s: %s", BOOST_CURRENT_FUNCTION, e.what());
	}
}

void
threaded_connection_validator::init(settings const &s) {
	assert(logger_);
	
	boost::function<void()> f = boost::bind(&threaded_connection_validator::thread_func, this);
	unsigned short threads = s.matcher_threads();
	for (unsigned short i = 0; i < threads; ++i) {
		create_thread(f);
	}
}
						

void
threaded_connection_validator::validate(connection_type_ptr conn, request_impl &req) {

	matcher_->check(req);

	request_provider_ptr rp_ptr(new request_provider());
	rp_ptr->req.swap(req);

	input_queue_.push(queue_item_type(rp_ptr, conn));
}

void
threaded_connection_validator::attach_logger(boost::intrusive_ptr<logger> const &log) {
	logger_ = log;
}



}} // namespaces
