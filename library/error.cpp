#include "acsetup.hpp"
#include "xiva/error.hpp"

#include <cstdio>
#include <cstdarg>

namespace xiva {

error::error(char const *format, ...)
{
	va_list args;
	va_start(args, format);
	vsnprintf(buffer_, message_size, format, args);
	va_end(args);
}

error::~error() throw () {
}

char const*
error::what() const throw () {
	return buffer_;
}

} // namespace
