#include "acsetup.hpp"

#include "python_matcher.hpp"
#include "python_request.hpp"
#include "interpreter_lock.hpp"

#include "xiva/request.hpp"

namespace xiva { namespace python {

python_matcher::python_matcher(py::object const &impl) :
	impl_(impl)
{
}

python_matcher::~python_matcher() {
}

bool
python_matcher::threaded() const {
	return true;
}

std::string
python_matcher::receiver(request const &req) const {
	interpreter_lock lock;
	python_request r(req);
	return py::call_method<std::string>(impl_.ptr(), "receiver", r);
}

}} // namespaces
