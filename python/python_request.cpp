#include "acsetup.hpp"
#include "python_request.hpp"

#include "xiva/request.hpp"
#include "request_class.hpp"

namespace xiva { namespace python {

python_request::python_request(request const &req) :
	req_(req)
{
}

py::str
python_request::url() const {
	return py::str(req_.url());
}

py::str
python_request::param(std::string const &name) const {
	return py::str(req_.param(name));
}

bool
python_request::has_param(std::string const &name) const {
	return req_.has_param(name);
}

py::list
python_request::params(std::string const &name) const {
	py::list result;
	enumeration<std::string>::ptr_type en = req_.params(name);
	while (!en->empty()) {
		result.append(en->next());
	}
	return result;
}

py::str
python_request::header(std::string const &name) const {
	return py::str(req_.header(name));
}

bool
python_request::has_header(std::string const &name) const {
	return req_.has_header(name);
}

py::str
python_request::cookie(std::string const &name) const {
	return py::str(req_.cookie(name));
}

bool
python_request::has_cookie(std::string const &name) const {
	return req_.has_cookie(name);
}

void 
register_request_class() throw () {

	py::class_<python_request> reg("request", py::no_init);
	reg.def("url", &python_request::url);

	reg.def("param", &python_request::param);
	reg.def("params", &python_request::params);
	reg.def("has_param", &python_request::has_param);

	reg.def("header", &python_request::header);
	reg.def("has_header", &python_request::has_header);

	reg.def("cookie", &python_request::cookie);
	reg.def("has_cookie", &python_request::has_cookie);
}

}} // namespaces
