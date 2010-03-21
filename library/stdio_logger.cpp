#include "acsetup.hpp"
#include "details/stdio_logger.hpp"

#include <cstdio>

#include "details/system_error.hpp"

namespace xiva { namespace details {

stdio_logger::stdio_logger()
{
}

stdio_logger::~stdio_logger() {
}

void
stdio_logger::info(char const *format, ...) {
	va_list args;
	va_start(args, format);
	print_message("[INFO]", format, args);
	va_end(args);
}

void
stdio_logger::debug(char const *format, ...) {
	va_list args;
	va_start(args, format);
	print_message("[DEBUG]", format, args);
	va_end(args);
}

void
stdio_logger::error(char const *format, ...) {
	va_list args;
	va_start(args, format);
	print_message("[ERROR]", format, args);
	va_end(args);
}

void
stdio_logger::print_message(char const *level, char const *format, va_list args) {
	char buffer[1024];
	int res = vsnprintf(buffer, sizeof(buffer), format, args);
	system_error::throw_unless(static_cast<std::size_t>(res) < sizeof(buffer));
	buffer[sizeof(buffer) - 1] = '\0';
	fprintf(stderr, "%s %s\n", level, buffer);
}

}} // namespaces
