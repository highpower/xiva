#include "acsetup.hpp"
#include "python_formatter.hpp"

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

	interpreter_lock lock;

	return py::extract<std::string>(py::call_method<py::str>(impl_.ptr(), "wrap_message", content));
}

std::string
python_formatter::ping_message() const {

	interpreter_lock lock;

	return py::extract<std::string>(py::call_method<py::str>(impl_.ptr(), "ping_message"));
}

}} // namespaces
