#include "acsetup.hpp"
#include "details/matcher_invoker.hpp"

#include <string>
#include <cassert>

#include "xiva/receiver_matcher.hpp"

#include "details/connection.hpp"
#include "details/request_impl.hpp"
#include "details/connection_data.hpp"

namespace xiva { namespace details {

matcher_invoker::matcher_invoker(connection_data const &data)
{
	matcher_ = data.matcher();
	assert(matcher_);
	assert(!matcher_->threaded());
}

matcher_invoker::~matcher_invoker() {
}

void
matcher_invoker::init(settings const &s) {
	(void) s;
}

void
matcher_invoker::attach_logger(boost::intrusive_ptr<logger> const &log) {
	(void) log;
}

void
matcher_invoker::invoke_matcher(matcher_invoker::connection_ptr_type conn, request_impl &req) {
	matcher_->check(req);
	std::string receiver = matcher_->receiver(req);
	conn->matched(receiver, matcher_->content_type());
}

}} // namespaces
