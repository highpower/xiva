#include "acsetup.hpp"
#include "python_handler.hpp"

#include <stdexcept>

#include "python_request.hpp"
#include "python_response.hpp"
#include "interpreter_lock.hpp"

#include "xiva/request.hpp"

namespace xiva { namespace python {

python_handler::python_handler(py::object const &impl) :
	impl_(impl)
{
}

python_handler::~python_handler() {
}

bool
python_handler::threaded() const {
	return true;
}

bool
python_handler::has_enough_data(request const &req) const {

	try {
		python_request request_adapter(req);
		interpreter_lock lock;

		return py::call_method<bool>(impl_.ptr(), "has_enough_data", request_adapter);
	}
	catch (std::exception const &) {
		throw;
	}
	catch (...) {
		throw std::runtime_error("caught unknown exception in python_handler::has_enough_data");
	}
}

std::string
python_handler::receiver(request const &req) const {

	try {
		python_request request_adapter(req);
		interpreter_lock lock;

		return py::call_method<std::string>(impl_.ptr(), "receiver", request_adapter);
	}
	catch (std::exception const &) {
		throw;
	}
	catch (...) {
		throw std::runtime_error("caught unknown exception in python_handler::receiver");
	}
}

void
python_handler::handle_response(request const &req, response &resp) {

	try {
		python_request request_adapter(req);
		python_response response_adapter(resp);
		interpreter_lock lock;

		py::call_method<void>(impl_.ptr(), "handle_response", request_adapter, response_adapter);
	}
	catch (std::exception const &) {
		throw;
	}
	catch (...) {
		throw std::runtime_error("caught unknown exception in handle_response");
	}
}

}} // namespaces
