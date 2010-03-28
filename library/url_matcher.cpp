#include "acsetup.hpp"
#include "details/url_matcher.hpp"

#include <iterator>

#include "xiva/request.hpp"
#include "xiva/http_error.hpp"
#include "details/range.hpp"
#include "details/urlencode.hpp"

namespace xiva { namespace details {

url_matcher::url_matcher()
{
}

url_matcher::~url_matcher() {
}

bool
url_matcher::threaded() const {
	return false;
}

char const*
url_matcher::content_type() const {
	return "text/plain";
}

std::string
url_matcher::receiver(request const &req) const {
	return receiver(req.url());
}

std::string
url_matcher::receiver(std::string const &url) const {

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

}} // namespaces
