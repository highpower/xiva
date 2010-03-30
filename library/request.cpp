#include "acsetup.hpp"
#include "xiva/request.hpp"
#include "details/request_impl.hpp"

namespace xiva {

request::request(details::request_impl const &impl) :
	impl_(impl)
{
}

request::~request() {
}

std::string const&
request::url() const {
	return impl_.url();
}

bool
request::has_param(std::string const &name) const {
	return impl_.has_param(name);
}

std::string const&
request::param(std::string const &name) const {
	return impl_.param(name);
}

enumeration<std::string>::ptr_type
request::params(std::string const &name) const {
	return impl_.params(name);
}

bool
request::has_header(std::string const &name) const {
	return impl_.has_header(name);
}

std::string const&
request::header(std::string const &name) const {
	return impl_.header(name);
}

bool
request::has_cookie(std::string const &name) const {
	return impl_.has_cookie(name);
}

std::string const&
request::cookie(std::string const &name) const {
	return impl_.cookie(name);
}

} // namespace
