#include "acsetup.hpp"
#include "python_request.hpp"

#include <stdexcept>
#include <boost/python/errors.hpp>

#include "xiva/request.hpp"
#include "request_class.hpp"

namespace xiva { namespace python {

python_request::python_request(request const &req) :
	req_(req) 
{
}

python_request::~python_request() {
}

py::str
python_request::uri() const {

	try {
		return py::str(req_.uri());
	}
	catch (std::exception const &e) {
		PyErr_SetString(PyExc_RuntimeError, e.what());
		boost::python::throw_error_already_set();
		throw;
	}
}

py::str
python_request::param(std::string const &name) const {

	try {
		return py::str(req_.param(name));
	}
	catch (std::exception const &e) {
		PyErr_SetString(PyExc_RuntimeError, e.what());
		boost::python::throw_error_already_set();
		throw;
	}
}

bool
python_request::has_param(std::string const &name) const {

	try {
		return req_.has_param(name);
	}
	catch (std::exception const &e) {
		PyErr_SetString(PyExc_RuntimeError, e.what());
		boost::python::throw_error_already_set();
		throw;
	}
}

py::list
python_request::params(std::string const &name) const {

	try {
		py::list result;
		enumeration<std::string>::ptr_type en = req_.params(name);
		while (!en->empty()) {
			result.append(en->next());
		}
		return result;
	}
	catch (std::exception const &e) {
		PyErr_SetString(PyExc_RuntimeError, e.what());
		boost::python::throw_error_already_set();
		throw;
	}
}

py::str
python_request::header(std::string const &name) const {

	try {
		return py::str(req_.header(name));
	}
	catch (std::exception const &e) {
		PyErr_SetString(PyExc_RuntimeError, e.what());
		boost::python::throw_error_already_set();
		throw;
	}
}

bool
python_request::has_header(std::string const &name) const {

	try {
		return req_.has_header(name);
	}
	catch (std::exception const &e) {
		PyErr_SetString(PyExc_RuntimeError, e.what());
		boost::python::throw_error_already_set();
		throw;
	}
}

py::str
python_request::cookie(std::string const &name) const {

	try {
		return py::str(req_.cookie(name));
	}
	catch (std::exception const &e) {
		PyErr_SetString(PyExc_RuntimeError, e.what());
		boost::python::throw_error_already_set();
		throw;
	}
}

bool
python_request::has_cookie(std::string const &name) const {

	try {
		return req_.has_cookie(name);
	}
	catch (std::exception const &e) {
		PyErr_SetString(PyExc_RuntimeError, e.what());
		boost::python::throw_error_already_set();
		throw;
	}
}

bool
python_request::is_websocket() const {
	try {
		return req_.is_websocket();
	}
	catch (std::exception const &e) {
		PyErr_SetString(PyExc_RuntimeError, e.what());
		boost::python::throw_error_already_set();
		throw;
	}
}

void
register_request_class() throw () {

	py::class_<python_request> reg("request", py::no_init);
	reg.def("uri", &python_request::uri);

	reg.def("param", &python_request::param);
	reg.def("params", &python_request::params);
	reg.def("has_param", &python_request::has_param);

	reg.def("header", &python_request::header);
	reg.def("has_header", &python_request::has_header);

	reg.def("cookie", &python_request::cookie);
	reg.def("has_cookie", &python_request::has_cookie);

	reg.def("is_websocket", &python_request::is_websocket);
}

}} // namespaces
