#include "acsetup.hpp"
#include "details/url_response_handler.hpp"

#include <iterator>

#include "xiva/request.hpp"
#include "xiva/http_error.hpp"
#include "details/range.hpp"
#include "details/urlencode.hpp"

namespace xiva { namespace details {

url_response_handler::url_response_handler()
{
}

url_response_handler::~url_response_handler() {
}

std::string
url_response_handler::receiver(std::string const &url) const {

	std::string::const_iterator s = url.begin();
	std::string::size_type spos = url.rfind('/');
	if (std::string::npos != spos) {
		std::advance(s, spos + 1);
	}
	std::string::size_type qpos = url.rfind('?');
	std::string::const_iterator q = url.begin();
	if (std::string::npos != qpos) {
		std::advance(q, qpos);
	}
	else {
		q = url.end();
	}
	std::string res = urldecode<std::string>(make_range(s, q));
	if (res.empty()) {
		throw http_error(http_error::not_found);
	}
	return res;
}

bool
url_response_handler::threaded() const {
	return false;
}

bool
url_response_handler::has_enough_data(request const &req) const {
	return "/" != req.url();
}

std::string
url_response_handler::receiver(request const &req) const {
	return receiver(req.url());
}

void
url_response_handler::handle_response(request const &req, response &resp) {
	// TODO: set content type on the response
}

}} // namespaces
