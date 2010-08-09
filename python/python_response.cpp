#include "acsetup.hpp"
#include "python_response.hpp"

#include <stdexcept>
#include <boost/python/errors.hpp>

#include "xiva/response.hpp"
#include "xiva/channel_info.hpp"
#include "response_class.hpp"

namespace xiva { namespace python {

python_response::python_response(response &resp) :
	resp_(resp)
{
}

python_response::~python_response() {
}

void
python_response::content_type(std::string const &type) {
	try {
		resp_.content_type(type);
	}
	catch (std::exception const &e) {
		PyErr_SetString(PyExc_RuntimeError, e.what());
		boost::python::throw_error_already_set();
	}
}

void
python_response::single_message(bool value) {
	try {
		resp_.single_message(value);
	}
	catch (std::exception const &e) {
		PyErr_SetString(PyExc_RuntimeError, e.what());
		boost::python::throw_error_already_set();
	}
}

void
python_response::content(std::string const &body) {
	try {
		resp_.content(body);
	}
	catch (std::exception const &e) {
		PyErr_SetString(PyExc_RuntimeError, e.what());
		boost::python::throw_error_already_set();
	}
}

void
python_response::formatter_id(std::string const &fmt_id) {
	try {
		resp_.formatter_id(fmt_id);
	}
	catch (std::exception const &e) {
		PyErr_SetString(PyExc_RuntimeError, e.what());
		boost::python::throw_error_already_set();
	}
}

void
python_response::formatter_by_channel(
		std::string const &channel_name, std::string const &channel_key, std::string const &channel_data,
		std::string const &fmt_id) {

	try {
		channel_info ch_info(channel_name, channel_key, channel_data);
		resp_.formatter_by_channel(ch_info, fmt_id);
	}
	catch (std::exception const &e) {
		PyErr_SetString(PyExc_RuntimeError, e.what());
		boost::python::throw_error_already_set();
	}
}

void
register_response_class() throw () {
	py::class_<python_response> reg("response", py::no_init);
	reg.def("content_type", &python_response::content_type);
	reg.def("single_message", &python_response::single_message);
	reg.def("content", &python_response::content);
	reg.def("formatter_id", &python_response::formatter_id);
	reg.def("formatter_by_channel", &python_response::formatter_by_channel);
}

}} // namespaces
