#include "acsetup.hpp"
#include "python_formatter_creator.hpp"

#include "python_formatter.hpp"
#include "python_request.hpp"
#include "interpreter_lock.hpp"

namespace xiva { namespace python {

python_formatter_creator::python_formatter_creator(py::object const &impl) :
	impl_(impl)
{
}

python_formatter_creator::~python_formatter_creator() {
}

std::auto_ptr<formatter>
python_formatter_creator::create(request const &req) const {

	python_request request_adapter(req);
	interpreter_lock lock;

	py::object formatter_impl = py::call_method<py::object>(impl_.ptr(), "create", request_adapter);
	return std::auto_ptr<formatter>(new python_formatter(formatter_impl));
}

}} // namespaces
