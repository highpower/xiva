#include "acsetup.hpp"
#include "details/http_constants.hpp"

namespace xiva { namespace details {

char const* const
http_constants_char::endl = "\r\n";

range<char const*> const
http_constants_char::range_endl = make_range("\r\n");

std::string const
http_constants_char::empty_string;

range<char const*> const
http_constants_char::get = make_range("get");

range<char const*> const
http_constants_char::range_name = make_range("range");

range<char const*> const
http_constants_char::cookie = make_range("cookie");

range<char const*> const
http_constants_char::param_delimiter = make_range(";&");

range<char const*> const
http_constants_char::protocol_version = make_range("http/1.1");

range<char const*> const
http_constants_char::policy_file_request = make_range("<policy-file-request/>");

range<char const*> const
http_constants_char::sec_websocket_key1 = make_range("sec-websocket-key1");

range<char const*> const
http_constants_char::headers_end = make_range("\r\n\r\n");

range<char const*> const
http_constants_char::nonstd_headers_end = make_range("\n\n");

}} // namespaces
