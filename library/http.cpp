#include "acsetup.hpp"
#include "details/http.hpp"

#include <cstdio>

#include "details/system_error.hpp"

namespace xiva { namespace details {

static char const* day_of_week_names[] = {
	"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
};

static char const* month_names[] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

http_date::http_date(boost::posix_time::ptime const &p) :
	when_(p)
{
}

std::ostream&
operator << (std::ostream &stream, http_date const &val) {

	char buffer[256];
	boost::posix_time::ptime const &p = val.when_;

	boost::gregorian::date gd = p.date();
	boost::gregorian::date::day_of_week_type dw = gd.day_of_week();
	boost::posix_time::time_duration td = p.time_of_day();

	int res = snprintf(buffer, sizeof(buffer), "Date: %s, %02ld %s %04ld %02ld:%02ld:%02ld GMT\r\n",
		day_of_week_names[dw.as_number() - 1], static_cast<long>(gd.day()), month_names[gd.month() - 1],
		static_cast<long>(gd.year()), static_cast<long>(td.hours()),
		static_cast<long>(td.minutes()), static_cast<long>(td.seconds()));
	system_error::throw_unless(res > 0);
	stream.write(buffer, res);
	return stream;
}

http_header::http_header(char const *name, char const *value) :
	name_(name), value_(value)
{
}

std::ostream&
operator << (std::ostream &stream, http_header const &val) {
	stream << val.name_ << ": " << val.value_;
	stream.write("\r\n", sizeof("\r\n") - 1);
	return stream;
}

http_header
http_header::server() {
	return http_header("Server", XIVA_PACKAGE_NAME "/" XIVA_PACKAGE_VERSION);
}

http_header
http_header::connection_close() {
	return http_header("Connection", "close");
}

http_header
http_header::connection_keep_alive() {
	return http_header("Connection", "keep-alive");
}

http_status::http_status(unsigned short status) :
	code_(status) 
{
}

std::ostream&
operator << (std::ostream &stream, http_status const &val) {
	stream << "HTTP/1.1 " << val.code_ << " " << http_status::str(val.code_);
	stream.write("\r\n", sizeof("\r\n") - 1);
	return stream;
}

char const*
http_status::str(unsigned short code) {

	switch (code) {
	case 200:
		return "OK";
	case 201:
		return "Created";
	case 202:
		return "Accepted";
	case 203:
		return "Non-Authoritative Information";
	case 204:
		return "No Content";

	case 301:
		return "Moved Permanently";
	case 302:
		return "Found";
	case 303:
		return "See Other";
	case 304:
		return "Not Modified";

	case 400:
		return "Bad Request";
	case 401:
		return "Unauthorized";
	case 402:
		return "PaymentRequired";
	case 403:
		return "Forbidden";
	case 404:
		return "Not Found";
	case 405:
		return "Method Not Allowed";
	case 416:
		return "Requested Range Not Satisfiable";

	case 500:
		return "Internal Server Error";
	case 501:
		return "Not Implemented";
	case 502:
		return "Bad Gateway";
	case 503:
		return "Service Unavailable";
	case 504:
		return "Gateway Timeout";
	case 505:
		return "HTTP Version Not Supported";
	}
	return "Unknown Status";
}

}} // namespaces
