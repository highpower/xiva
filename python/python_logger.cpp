#include "acsetup.hpp"
#include "python_logger.hpp"

#include <cstdio>

#include "interpreter_lock.hpp"
#include "details/system_error.hpp"

namespace xiva { namespace python {

python_logger::python_logger(py::object const &impl) :
	impl_(impl)
{
}

python_logger::~python_logger() {
}

void
python_logger::info(char const *format, ...) {

	va_list args;
	va_start(args, format);

	invoke("info", format, args);
	va_end(args);
}

void
python_logger::debug(char const *format, ...) {

	va_list args;
	va_start(args, format);

	invoke("debug", format, args);
	va_end(args);
}

void
python_logger::error(char const *format, ...) {

	va_list args;
	va_start(args, format);

	invoke("error", format, args);
	va_end(args);
}

void
python_logger::invoke(char const *method, char const *format, va_list args) {
	char buffer[1024];
	int res = vsnprintf(buffer, sizeof(buffer) - 1, format, args);
	details::system_error::throw_unless(static_cast<std::size_t>(res) < sizeof(buffer));
	buffer[sizeof(buffer) - 1] = '\0';

	interpreter_lock lock;
	py::call_method<void>(impl_.ptr(), method, buffer);
}

}} // namespaces
