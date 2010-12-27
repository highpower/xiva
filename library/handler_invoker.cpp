#include "acsetup.hpp"
#include "details/handler_invoker.hpp"

#include <string>
#include <cassert>

#include "xiva/request.hpp"
#include "xiva/response.hpp"
#include "xiva/http_error.hpp"
#include "xiva/response_handler.hpp"

#include "details/connection.hpp"
#include "details/connection_data.hpp"

namespace xiva { namespace details {

handler_invoker::handler_invoker(asio::io_service::strand &st, connection_data const &data)
{
	(void)st;

	handler_ = data.handler();
	assert(handler_);
	assert(!handler_->threaded());
}

handler_invoker::~handler_invoker() {
}

void
handler_invoker::init(settings const &s) {
	(void) s;
}

void
handler_invoker::finish() {
}

void
handler_invoker::wait_for_complete() {
}

void
handler_invoker::attach_logger(boost::intrusive_ptr<logger> const &log) {
	(void) log;
}

void
handler_invoker::invoke_handler(handler_invoker::connection_ptr_type conn, request_impl &req, response_impl &resp) {

	request request_adapter(req);
	if (!handler_->has_enough_data(request_adapter)) {
		throw http_error(http_error::forbidden);
	}	
	std::string receiver = handler_->receiver(request_adapter);
	conn->name(receiver);
	
	response response_adapter(resp);
	handler_->handle_response(request_adapter, response_adapter);
	conn->handled(req, resp);
}

}} // namespaces
