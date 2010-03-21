#include "acsetup.hpp"
#include "details/connection_validator.hpp"

#include <string>

#include "details/connection_data.hpp"
#include "details/request_impl.hpp"



namespace xiva { namespace details {


connection_validator::connection_validator(connection_data const &data) {

	boost::intrusive_ptr<receiver_matcher> matcher = data.matcher();
	assert(matcher);
	assert(!matcher->threaded());
	matcher_ = matcher;
}

connection_validator::~connection_validator() {
}

bool
connection_validator::validate(connection_type_ptr conn, request_impl &req) {

	matcher_->check(req);
        std::string receiver = matcher_->receiver(req);
        conn->validate_result(receiver);
        return true;
}

void
connection_validator::attach_logger(boost::intrusive_ptr<logger> const &log) {
	(void)log;
}

void
connection_validator::init(settings const &s) {
	(void)s;
}


}} // namespaces
