#include "acsetup.hpp"
#include "xiva/http_error.hpp"
#include "details/http.hpp"

namespace xiva {

unsigned short const http_error::bad_request = 400;
unsigned short const http_error::unauthorized = 401;
unsigned short const http_error::payment_required = 402;
unsigned short const http_error::forbidden = 403;
unsigned short const http_error::not_found = 404;
unsigned short const http_error::method_not_allowed = 405;
unsigned short const http_error::entity_too_large = 413;
unsigned short const http_error::range_not_satisfiable = 416;

unsigned short const http_error::internal_error = 500;
unsigned short const http_error::not_implemented = 501;
unsigned short const http_error::bad_gateway = 502;
unsigned short const http_error::service_unavailable = 503;
unsigned short const http_error::gateway_timeout = 504;
unsigned short const http_error::version_not_supported = 505;

http_error::http_error(unsigned short code) :
	code_(code)
{
}

http_error::~http_error() throw () {
}

char const*
http_error::what() const throw () {
	return details::http_status::str(code_);
}

unsigned short
http_error::code() const {
	return code_;
}

} // namespace

