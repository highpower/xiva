#include "acsetup.hpp"

#include <cstdarg>
#include <syslog.h>

#include "xiva/config.hpp"
#include "xiva/logger.hpp"
#include "xiva/settings.hpp"
#include "xiva/component_set.hpp"

namespace xiva { namespace standard {

class syslog_logger : public logger {

public:
	syslog_logger(std::string const &name);
	virtual ~syslog_logger();

	virtual void info(char const *format, ...);
	virtual void debug(char const *format, ...);
	virtual void error(char const *format, ...);

private:
	syslog_logger(syslog_logger const &);
	syslog_logger& operator = (syslog_logger const &);

private:
	std::string const name_;
};

syslog_logger::syslog_logger(std::string const &name) :
	name_(name)
{
	openlog(name_.c_str(), 0, LOG_DAEMON);
}

syslog_logger::~syslog_logger() {
	closelog();
}

void
syslog_logger::info(char const *format, ...) {
	va_list args;
	va_start(args, format);
	vsyslog(LOG_INFO, format, args);
	va_end(args);
}

void
syslog_logger::debug(char const *format, ...) {
	va_list args;
	va_start(args, format);
	vsyslog(LOG_DEBUG, format, args);
	va_end(args);
}

void
syslog_logger::error(char const *format, ...) {
	va_list args;
	va_start(args, format);
	vsyslog(LOG_ERR, format, args);
	va_end(args);
}

extern "C" XIVA_API void
xiva_register_module(settings const &s, component_set &cs) {
	std::string logname  = s.value("/xiva/logger/name");
	boost::intrusive_ptr<logger> log(new syslog_logger(logname));
	cs.attach_logger(log);
}

}} // namespaces
