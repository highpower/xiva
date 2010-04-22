#include "acsetup.hpp"
#include "python_response.hpp"

#include "xiva/response.hpp"
#include "response_class.hpp"

namespace xiva { namespace python {

python_response::python_response(response &resp) :
	resp_(resp)
{
}

python_response::~python_response() {
}

void
python_response::content_type(py::str type) {
	resp_.content_type(py::extract<std::string>(type));
}

void
python_response::single_message(bool value) {
	resp_.single_message(value);
}

void
python_response::content(py::str body) {
	resp_.content(py::extract<std::string>(body));
}


void
register_response_class() throw () {
	py::class_<python_response> reg("response", py::no_init);
	reg.def("content_type", &python_response::content_type);
	reg.def("single_message", &python_response::single_message);
	reg.def("content", &python_response::content);
}

}} // namespaces
