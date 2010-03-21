#include "acsetup.hpp"
#include "python_listener.hpp"
#include "gil.hpp"

namespace xiva { namespace python {

python_listener::python_listener(py::object const &impl) :
	impl_(impl)
{
}

python_listener::~python_listener() {
}

void
python_listener::connection_opened(std::string const &to, globals::connection_id const &id) throw (std::exception) {
	interpreter_lock lock;
	py::call_method<void>(impl_.ptr(), "connection_opened", to, id);
}

void
python_listener::connection_closed(std::string const &to, globals::connection_id const &id) throw (std::exception) {
	interpreter_lock lock;
	py::call_method<void>(impl_.ptr(), "connection_closed", to, id);
}
	
}} // namespaces
