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
python_response::content_type(std::string const &type) {
	resp_.content_type(type);
}

void
python_response::single_message(bool value) {
	resp_.single_message(value);
}

void
python_response::content(std::string const &body) {
	resp_.content(body);
}

void
python_response::formatter_id(std::string const &fmt_id) {
	resp_.formatter_id(fmt_id);
}

void
python_response::formatter_by_channel(std::string const &channel_id, std::string const &fmt_id) {
	resp_.formatter_by_channel(channel_id, fmt_id);
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
