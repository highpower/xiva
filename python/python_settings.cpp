#include "acsetup.hpp"
#include "python_settings.hpp"

namespace xiva { namespace python {

python_settings::python_settings(py::object const &impl) :
	address_("127.0.0.1"), port_(8080), backlog_(1024), 
	read_timeout_(5000), write_timeout_(5000), inactive_timeout_(60000), validate_timeout_(15000),
	listener_threads_(5), validator_threads_(5)
{
	init_if_exists(impl, "port", port_);
	init_if_exists(impl, "backlog", backlog_);
	init_if_exists(impl, "read_timeout", read_timeout_);
	init_if_exists(impl, "write_timeout", write_timeout_);
	init_if_exists(impl, "inactive_timeout", inactive_timeout_);
	init_if_exists(impl, "validate_timeout", validate_timeout_);
	init_if_exists(impl, "listener_threads", listener_threads_);
	init_if_exists(impl, "validator_threads", validator_threads_);
	init_if_exists(impl, "address", address_);
}

python_settings::~python_settings() {
}
	
std::string
python_settings::address() const {
	return address_;
}

unsigned short
python_settings::port() const {
	return port_;
}

unsigned short
python_settings::backlog() const {
	return backlog_;
}

unsigned short
python_settings::read_timeout() const {
	return read_timeout_;
}

unsigned short
python_settings::write_timeout() const {
	return write_timeout_;
}

unsigned int
python_settings::inactive_timeout() const {
	return inactive_timeout_;
}

unsigned int
python_settings::validate_timeout() const {
	return validate_timeout_;
}

unsigned short
python_settings::listener_threads() const {
	return listener_threads_;
}

unsigned short
python_settings::validator_threads() const {
	return validator_threads_;
}


std::string
python_settings::value(std::string const &name) const {
}

enumeration<std::string>::ptr_type
python_settings::value_list(std::string const &prefix) const {
}
	
}} // namespaces
