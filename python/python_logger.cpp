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
		items_.finish();
		join_all();
	}
	catch (std::exception const &) {
	}
}

void
python_logger::start() {
	create_thread(boost::bind(&python_logger::thread_func, this));
}

void
python_logger::thread_func() {

	try {
		items_type items;
		while (items_.pop_all(items)) {

			interpreter_try_lock lock;
			if (!lock.acquired()) {
				return;
			}
			process_items(items);
		}
	}
	catch (...) {
		items_.finish(); // suppress all exceptions and stop logger
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
python_logger::process_items(items_type const &items) const {

	for (items_type::const_iterator i = items.begin(), end = items.end(); i != end; ++i) {
		char level = i->first;
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
		py::call_method<void>(impl_.ptr(), method, i->second.c_str());
	}
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
