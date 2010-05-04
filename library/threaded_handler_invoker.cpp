#include "acsetup.hpp"
#include "details/threaded_handler_invoker.hpp"

#include <string>
#include <exception>
#include <boost/bind.hpp>
#include <boost/current_function.hpp>
#include <boost/function.hpp>

#include "xiva/logger.hpp"
#include "xiva/request.hpp"
#include "xiva/response.hpp"
#include "xiva/settings.hpp"
#include "xiva/response_handler.hpp"

#include "details/request_impl.hpp"
#include "details/response_impl.hpp"
#include "details/connection_data.hpp"
#include "details/threaded_shared.hpp"
#include "details/threaded_connection.hpp"

namespace xiva { namespace details {

class request_holder : public threaded_shared {

public:
	request_holder();
	virtual ~request_holder();
	
	request_impl& request();
	response_impl& response();
	void attach(request_impl &req, response_impl &resp);

private:
	request_impl req_;
	response_impl resp_;
};

threaded_handler_invoker::threaded_handler_invoker(asio::io_service &io, connection_data const &data) : 
	io_(io), strand_(io)
{
	boost::intrusive_ptr<response_handler> handler = data.handler();
	assert(handler);
	assert(handler->threaded());
	handler_ = handler;
}

threaded_handler_invoker::~threaded_handler_invoker() {
	try {
		finish();
		join_all();
	}
	catch (std::exception const &e) {
		(void) e;
	}
}

void
threaded_handler_invoker::pop() {
	std::deque<item_type> handled;
	pop_handled(handled);
	try {
		for (std::deque<item_type>::iterator i = handled.begin(), end = handled.end(); i != end; ++i) {
			request_impl const &req = i->first->request();
			response_impl const &resp = i->first->response();
			i->second->handled(req, resp);
		}
	}
	catch (std::exception const &e) {
		logger_->error("exception was caught in %s: %s", BOOST_CURRENT_FUNCTION, e.what());
	}
}

void
threaded_handler_invoker::thread_func() {
	item_type item;
	while (input_queue_.pop(item)) {
		try {
			request request_adapter(item.first->request());
			std::string receiver = handler_->receiver(request_adapter);
			item.second->name(receiver);
			response response_adapter(item.first->response());
			handler_->handle_response(request_adapter, response_adapter);
			handled(item);
			strand_.dispatch(boost::bind(&threaded_handler_invoker::pop, this));
		}
		catch (std::exception const &e) {
			logger_->error("exception was caught in %s: %s", BOOST_CURRENT_FUNCTION, e.what());
		}
	}
}

void
threaded_handler_invoker::finish() {
	input_queue_.finish();
}

void
threaded_handler_invoker::init(settings const &s) {
	assert(logger_);
	boost::function<void()> f = boost::bind(&threaded_handler_invoker::thread_func, this);
	unsigned short threads = s.handler_threads();
	for (unsigned short i = 0; i < threads; ++i) {
		create_thread(f);
	}
}

void
threaded_handler_invoker::attach_logger(boost::intrusive_ptr<logger> const &log) {
	logger_ = log;
}

void
threaded_handler_invoker::invoke_handler(threaded_handler_invoker::connection_ptr_type conn, request_impl &req, response_impl &resp) {
	request r(req);
	if (!handler_->has_enough_data(r)) {
		throw http_error(http_error::forbidden);
	}
	
	holder_ptr_type holder(new request_holder());
	holder->attach(req, resp);
	input_queue_.push(item_type(holder, conn));
}

void
threaded_handler_invoker::handled(threaded_handler_invoker::item_type const &item) {
	boost::mutex::scoped_lock sl(mutex_);
	handled_.push_back(item);
}

void
threaded_handler_invoker::pop_handled(std::deque<threaded_handler_invoker::item_type> &items) {
	items.clear();
	boost::mutex::scoped_lock sl(mutex_);
	items.swap(handled_);
}

request_holder::request_holder()
{
}

request_holder::~request_holder() {
}

request_impl&
request_holder::request() {
	return req_;
}

response_impl&
request_holder::response() {
	return resp_;
}

void
request_holder::attach(request_impl &req, response_impl &resp) {
	req_.swap(req);
	resp_.swap(resp);
}

}} // namespaces
