#include "acsetup.hpp"
#include "python_settings.hpp"
#include "python_enumeration.hpp"
#include "interpreter_lock.hpp"

namespace xiva { namespace python {

template <typename Result> inline Result
python_settings::get(char const *method, Result const &defval) const {
	if (!impl_) {
		return defval;
	}
	try {
		interpreter_lock lock;
		py::object func = impl_.attr(method);
		if (func) {
			return py::call<Result>(func.ptr());
		}
	}
	catch (...) {
		// suppress any exception
	}
	return defval;
}



python_settings::python_settings(py::object const &impl) :
	impl_(impl)
{
}

python_settings::~python_settings() {
}

std::string
python_settings::address() const {
	return get<std::string>("address", "127.0.0.1");
}

unsigned short
python_settings::port() const {
	return get<unsigned short>("port", 8080);
}

unsigned short
python_settings::backlog() const {
	return get<unsigned short>("backlog", 1024);
}

unsigned int
python_settings::read_timeout() const {
	return get<unsigned int>("read_timeout", 5000);
}

unsigned int
python_settings::write_timeout() const {
	return get<unsigned int>("write_timeout", 5000);
}

unsigned int
python_settings::inactive_timeout() const {
	return get<unsigned int>("inactive_timeout", 60000);
}

unsigned short
python_settings::handler_threads() const {
	return get<unsigned short>("handler_threads", 5);
}

unsigned short
python_settings::listener_threads() const {
	return get<unsigned short>("listener_threads", 5);
}

std::string
python_settings::policy_file_name() const {
	return get<std::string>("policy_file_name", "");
}

std::string
python_settings::value(char const *name) const {
	if (impl_) {
		try {
			interpreter_lock lock;
			return py::call_method<std::string>(impl_.ptr(), "value", name);
		}
		catch (...) {
			// suppress any exception
		}
	}
	return std::string();
}

enumeration<std::string>::ptr_type
python_settings::value_list(char const *prefix) const {
	if (impl_) {
		try {
			interpreter_lock lock;
			py::tuple tuple = py::call_method<py::tuple>(impl_.ptr(), "value_list", prefix);
			return enumeration<std::string>::ptr_type(new python_enumeration<std::string>(tuple));
		}
		catch (...) {
			// suppress any exception
		}
	}
	return enumeration<std::string>::ptr_type(new python_enumeration<std::string>());
}

}} // namespaces
