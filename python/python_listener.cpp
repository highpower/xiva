#include "acsetup.hpp"
#include "python_listener.hpp"

#include <stdexcept>
#include <boost/lexical_cast.hpp>

#include "interpreter_lock.hpp"

namespace xiva { namespace python {

static void
throw_listener_unknown_error(char const *method, std::string const &to, globals::connection_id id) {
	std::string msg = "caught unknown exception in python_listener::";
	msg.append(method).append(" for ").append(to);
	if (id) {
		msg.append(" ").append(boost::lexical_cast<std::string>(id));
	}
	throw std::runtime_error(msg);
}

python_listener::python_listener(py::object const &impl) :
	impl_(impl)
{
}

python_listener::~python_listener() {
}

void
python_listener::connection_opened(std::string const &to, globals::connection_id id) throw (std::exception) {

	try {
		interpreter_lock lock;
		py::call_method<void>(impl_.ptr(), "connection_opened", to, id);
	}
	catch (std::exception const &) {
		throw;
	}
	catch (...) {
		throw_listener_unknown_error("connection_opened", to, id);
	}
}

void
python_listener::connection_closed(std::string const &to, globals::connection_id id) throw (std::exception) {

	try {
		interpreter_lock lock;
		py::call_method<void>(impl_.ptr(), "connection_closed", to, id);
	}
	catch (std::exception const &) {
		throw;
	}
	catch (...) {
		throw_listener_unknown_error("connection_closed", to, id);
	}
}

void
python_listener::disconnected(std::string const &to) throw (std::exception) {

	try {
		interpreter_lock lock;
		py::call_method<void>(impl_.ptr(), "disconnected", to);
	}
	catch (std::exception const &) {
		throw;
	}
	catch (...) {
		throw_listener_unknown_error("disconnected", to, 0);
	}
}

}} // namespaces
