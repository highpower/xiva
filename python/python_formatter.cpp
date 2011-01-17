#include "acsetup.hpp"
#include "python_formatter.hpp"

#include <stdexcept>

#include "cleanup_list.hpp"
#include "interpreter_lock.hpp"

namespace xiva { namespace python {

python_formatter::python_formatter(py::object const &impl, cleanup_list &cleanup) :
	impl_(boost::shared_ptr<py::object>(new py::object(impl))), cleanup_(cleanup)
{
}

python_formatter::~python_formatter() {
	cleanup_.push_ref(impl_);
}

std::string
python_formatter::wrap_message(std::string const &content) const {

	try {
		interpreter_lock lock;
		return py::extract<std::string>(py::call_method<py::str>(impl_->ptr(), "wrap_message", content));
	}
	catch (std::exception const &) {
		throw;
	}
	catch (...) {
		throw std::runtime_error("caught unknown exception in python_formatter::wrap_message");
	}
}

}} // namespaces
