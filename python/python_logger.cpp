#include "acsetup.hpp"
#include "python_logger.hpp"

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
	
	interpreter_lock lock;
	invoke("info", format, args);
	va_end(args);
}

void
python_logger::debug(char const *format, ...) {

	va_list args;
	va_start(args, format);
	
	interpreter_lock lock;
	invoke("debug", format, args);
	va_end(args);
}

void
python_logger::error(char const *format, ...) {

	va_list args;
	va_start(args, format);
	
	interpreter_lock lock;
	invoke("error", format, args);
	va_end(args);
}

void
python_logger::invoke(char const *method, char const *format, va_list args) {
	char buffer[1024];
	int res = vsnprintf(buffer, sizeof(buffer) - 1, format, args);
	details::system_error::throw_unless(res > 0 && res < static_cast<int>(sizeof(buffer)));
	py::call_method<void>(impl_.ptr(), method, buffer);
}

}} // namespaces
