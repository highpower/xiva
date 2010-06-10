#include "acsetup.hpp"
#include "details/http_constants.hpp"

namespace xiva { namespace details {

char const* const
http_constants<const char>::endl = "\r\n";

std::string const
http_constants<const char>::empty_string;

range<char const*> const
http_constants<const char>::get = make_range("get");

range<char const*> const
http_constants<const char>::range_name = make_range("range");

range<char const*> const
http_constants<const char>::cookie = make_range("cookie");

range<char const*> const
http_constants<const char>::param_delimiter = make_range(";&");

range<char const*> const
http_constants<const char>::protocol_version = make_range("http/1.1");

range<char const*> const
http_constants<const char>::policy_file_request = make_range("<policy-file-request/>");

range<char const*> const
http_constants<const char>::reversed_headers_end = make_range("\n\r\n\r");

range<char const*> const
http_constants<const char>::reversed_nonstd_headers_end = make_range("\n\n");


char const* const
http_constants<char>::endl = "\r\n";

std::string const
http_constants<char>::empty_string;

range<char const*> const
http_constants<char>::get = make_range("get");

range<char const*> const
http_constants<char>::range_name = make_range("range");

range<char const*> const
http_constants<char>::cookie = make_range("cookie");

range<char const*> const
http_constants<char>::param_delimiter = make_range(";&");

range<char const*> const
http_constants<char>::protocol_version = make_range("http/1.1");

range<char const*> const
http_constants<char>::policy_file_request = make_range("<policy-file-request/>");

range<char const*> const
http_constants<char>::reversed_headers_end = make_range("\n\r\n\r");

range<char const*> const
http_constants<char>::reversed_nonstd_headers_end = make_range("\n\n");

}} // namespaces
