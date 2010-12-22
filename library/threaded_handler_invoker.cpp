#include "acsetup.hpp"
#include "details/threaded_handler_invoker.hpp"

#include <string>
#include <exception>
#include <boost/bind.hpp>
#include <boost/current_function.hpp>
#include <boost/function.hpp>

#include "xiva/http_error.hpp"
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
	void set_error_msg(char const *msg);
	void set_http_error_code(unsigned short code);
	
	void handled(connection_base &conn) const;

private:
	request_impl req_;
	response_impl resp_;
	std::string error_msg_;
	unsigned short http_code_;
};

threaded_handler_invoker::threaded_handler_invoker(asio::io_service::strand &st, connection_data const &data) : 
	strand_(st)
{
	boost::intrusive_ptr<response_handler> handler = data.handler();
	assert(handler);
	assert(handler->threaded());
	handler_ = handler;
}

threaded_handler_invoker::~threaded_handler_invoker() {
	try {
		input_queue_.finish();
		join_all();
	}
	catch (std::exception const &e) {
		(void) e;
	}
}

void
threaded_handler_invoker::pop() {
	items_type handled;
	pop_handled(handled);
	handle_items(handled);
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
		}
		catch (http_error const &e) {
			logger_->error("http error was caught in %s: %s", BOOST_CURRENT_FUNCTION, e.what());
			item.first->set_http_error_code(e.code());
		}
		catch (std::exception const &e) {
			logger_->error("exception was caught in %s: %s", BOOST_CURRENT_FUNCTION, e.what());
			item.first->set_error_msg(e.what());
		}
		catch (...) {
			logger_->error("unknown exception was caught in %s", BOOST_CURRENT_FUNCTION);
			item.first->set_error_msg("unknown exception while invoking handler");
		}
		handled(item);
		strand_.dispatch(boost::bind(&threaded_handler_invoker::pop, this));
	}
}

void
threaded_handler_invoker::finish() {
	input_queue_.finish();
	pop();

	items_type unhandled;
	input_queue_.swap_data(unhandled);

	for (items_type::iterator i = unhandled.begin(), end = unhandled.end(); i != end; ++i) {
		i->first->set_error_msg("finish");
	}
	handle_items(unhandled);
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
	if (!input_queue_.push(item_type(holder, conn))) {
		throw std::runtime_error("finish");
	}
}

void
threaded_handler_invoker::handle_items(items_type &items) const {
	try {
		for (items_type::iterator i = items.begin(), end = items.end(); i != end; ++i) {
			i->first->handled(*(i->second));
		}
	}
	catch (std::exception const &e) {
		logger_->error("exception was caught in %s: %s", BOOST_CURRENT_FUNCTION, e.what());
	}
}

void
threaded_handler_invoker::handled(threaded_handler_invoker::item_type const &item) {
	boost::mutex::scoped_lock sl(mutex_);
	handled_.push_back(item);
}

void
threaded_handler_invoker::pop_handled(items_type &items) {
	items.clear();
	boost::mutex::scoped_lock sl(mutex_);
	items.swap(handled_);
}

request_holder::request_holder() : http_code_(0)
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

void
request_holder::set_error_msg(char const *msg) {
	error_msg_.assign(msg);
}


void
request_holder::set_http_error_code(unsigned short code) {
	http_code_ = code;
}

void
request_holder::handled(connection_base &conn) const {
	if (!http_code_ && error_msg_.empty()) {
		conn.handled(req_, resp_);
	}
	else {
		conn.handled_errors(req_, resp_, http_code_, error_msg_);
	}
}


}} // namespaces
