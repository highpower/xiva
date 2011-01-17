#include "acsetup.hpp"
#include "python_ping_formatter.hpp"

#include <stdexcept>

#include "cleanup_list.hpp"
#include "interpreter_lock.hpp"

namespace xiva { namespace python {

python_ping_formatter::python_ping_formatter(py::object const &impl, cleanup_list &cleanup) :
	impl_(boost::shared_ptr<py::object>(new py::object(impl))), cleanup_(cleanup)
{
}

python_ping_formatter::~python_ping_formatter() {
	cleanup_.push_ref(impl_);
}

std::string
python_ping_formatter::message() const {

	try {
		interpreter_lock lock;
		return py::extract<std::string>(py::call_method<py::str>(impl_->ptr(), "ping_message"));
	}
	catch (std::exception const &) {
		throw;
	}
	catch (...) {
		throw std::runtime_error("caught unknown exception in python_formatter::ping_message");
	}
}

std::string
python_ping_formatter::message_for_single() const {

	try {
		interpreter_lock lock;
		return py::extract<std::string>(py::call_method<py::str>(impl_->ptr(), "ping_message_for_single"));
	}
	catch (std::exception const &) {
		throw;
	}
	catch (...) {
		throw std::runtime_error("caught unknown exception in python_formatter::ping_message_for_single");
	}
}

}} // namespaces
