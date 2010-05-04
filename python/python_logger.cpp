#include "acsetup.hpp"
#include "python_logger.hpp"

#include <cstdio>

#include "interpreter_lock.hpp"
#include "details/system_error.hpp"

namespace xiva { namespace python {

static char const *INFO = "info";
static char const *DEBUG = "debug";
static char const *ERROR = "error";

python_logger::python_logger(py::object const &impl) :
	impl_(impl)
{
}

python_logger::~python_logger() {
	try {
		finish();
		join_all();
	}
	catch (std::exception const &e) {
		(void) e;
	}
}

void
python_logger::start() {
	create_thread(boost::bind(&python_logger::thread_func, this));
}

void
python_logger::finish() {
	items_.finish();
}

void
python_logger::thread_func() {
	queue_item_type item;
	while (items_.pop(item)) {
		char level = item.first;
		char const *method = DEBUG;
		if (level == *INFO) {
			method = INFO;
		}
		else if (level == *ERROR) {
			method = ERROR;
		}
		/* else if (level == *DEBUG) {
			method = DEBUG;
		} else {
			wtf? ;)
		} */

		interpreter_lock lock;
		py::call_method<void>(impl_.ptr(), method, item.second.c_str());
	}
}

void
python_logger::info(char const *format, ...) {

	va_list args;
	va_start(args, format);

	invoke(*INFO, format, args);
	va_end(args);
}

void
python_logger::debug(char const *format, ...) {

	va_list args;
	va_start(args, format);

	invoke(*DEBUG, format, args);
	va_end(args);
}

void
python_logger::error(char const *format, ...) {

	va_list args;
	va_start(args, format);

	invoke(*ERROR, format, args);
	va_end(args);
}

void
python_logger::invoke(char level, char const *format, va_list args) {
	if (!items_.finished()) {
		char buffer[1024];
		int res = vsnprintf(buffer, sizeof(buffer) - 1, format, args);
		details::system_error::throw_unless(static_cast<std::size_t>(res) < sizeof(buffer));
		buffer[sizeof(buffer) - 1] = '\0';

		items_.push(queue_item_type(level, std::string(&buffer[0])));
	}
}

}} // namespaces
