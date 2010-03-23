#include "acsetup.hpp"

#include <cstring>

#include "details/http_constants.hpp"

namespace xiva { namespace details {

char const* const
http_constants::endl = "\r\n";

std::string const
http_constants::empty_string;

range<char const*> const
http_constants::get = make_range("get");

range<char const*> const
http_constants::range_name = make_range("range");

range<char const*> const
http_constants::cookie = make_range("cookie");

range<char const*> const
http_constants::param_delimiter = make_range(";&");

range<char const*> const
http_constants::protocol_version = make_range("http/1.1");

range<char const*> const
http_constants::policy_file_request = make_range("<policy-file-request/>");

range<char const*> const
http_constants::reversed_headers_end = make_range("\n\r\n\r");

range<char const*> const
http_constants::reversed_nonstd_headers_end = make_range("\n\n");

}} // namespaces
