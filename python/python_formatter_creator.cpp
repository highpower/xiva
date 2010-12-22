#include "acsetup.hpp"
#include "python_formatter_creator.hpp"

#include <stdexcept>

#include "python_formatter.hpp"
#include "python_request.hpp"
#include "interpreter_lock.hpp"

#include "cleanup_list.hpp"

namespace xiva { namespace python {

python_formatter_creator::python_formatter_creator(py::object const &impl, cleanup_list &cleanup) :
	impl_(boost::shared_ptr<py::object>(new py::object(impl))), cleanup_(cleanup)
{
}

python_formatter_creator::~python_formatter_creator() {
	cleanup_.push_ref(impl_);
}

std::auto_ptr<formatter>
python_formatter_creator::create(request const &req) const {

	try {
		python_request request_adapter(req);
		interpreter_lock lock;

		py::object formatter_impl = py::call_method<py::object>(impl_->ptr(), "create", request_adapter);
		return std::auto_ptr<formatter>(new python_formatter(formatter_impl, cleanup_));
	}
	catch (std::exception const &) {
		throw;
	}
	catch (...) {
		throw std::runtime_error("caugth unknown exception in python_formatter_creator::create");
	}
}

}} // namespaces
