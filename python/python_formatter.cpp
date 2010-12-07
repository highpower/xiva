#include "acsetup.hpp"
#include "python_formatter.hpp"

#include <stdexcept>

#include "interpreter_lock.hpp"

namespace xiva { namespace python {

python_formatter::python_formatter(py::object const &impl) :
	impl_(impl)
{
}

python_formatter::~python_formatter() {
}

std::string
python_formatter::wrap_message(std::string const &content) const {

	try {
		interpreter_lock lock;
		return py::extract<std::string>(py::call_method<py::str>(impl_.ptr(), "wrap_message", content));
	}
	catch (std::exception const &) {
		throw;
	}
	catch (...) {
		throw std::runtime_error("caugth unknown exception in python_formatter::wrap_message");
	}
}

std::string
python_formatter::ping_message() const {

	try {
		interpreter_lock lock;
		return py::extract<std::string>(py::call_method<py::str>(impl_.ptr(), "ping_message"));
	}
	catch (std::exception const &) {
		throw;
	}
	catch (...) {
		throw std::runtime_error("caugth unknown exception in python_formatter::ping_message");
	}
}

std::string
python_formatter::ping_message_for_single() const {

	try {
		interpreter_lock lock;
		return py::extract<std::string>(py::call_method<py::str>(impl_.ptr(), "ping_message_for_single"));
	}
	catch (std::exception const &) {
		throw;
	}
	catch (...) {
		throw std::runtime_error("caugth unknown exception in python_formatter::ping_message_for_single");
	}
}

}} // namespaces
