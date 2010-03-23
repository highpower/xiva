#include "acsetup.hpp"
#include "variable_map.hpp"
#include "xiva/error.hpp"

namespace xiva { namespace daemon {

variable_map::variable_map()
{
}

variable_map::~variable_map() {
}

void
variable_map::resolve_variables(std::string &str) {
	resolve_variables(str, 0, str.size());
}

void
variable_map::add_variable(std::string const &name, std::string const &value) {
	std::map<std::string, std::string>::iterator i = vars_.find(name);
	if (vars_.end() != i) {
		throw error("duplicate variable %s", name.c_str());
	}
	vars_[name] = value;
}

std::string const&
variable_map::find_variable(std::string const &name) const {
	std::map<std::string, std::string>::const_iterator i = vars_.find(name);
	if (vars_.end() == i) {
		throw error("nonexistent variable %s", name.c_str());
	}
	return i->second;
}

void
variable_map::resolve_variables(std::string &str, std::string::size_type begin, std::string::size_type end) {
	
}

}} // namespaces
