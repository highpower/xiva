#include "acsetup.hpp"
#include "xml_variable_map.hpp"
#include "xiva/error.hpp"

namespace xiva { namespace daemon {

xml_variable_map::xml_variable_map() :
	regex_("\\$\\{([A-Za-z][A-Za-z0-9\\-]*)\\}")
{
}

xml_variable_map::~xml_variable_map() {
}

void
xml_variable_map::resolve_variables(std::string &str) {
	boost::smatch res;
	while (boost::regex_search(str, res, regex_)) {
		if (2 == res.size()) {
			std::string key(res[1].first, res[1].second);
			str.replace(res.position((boost::smatch::size_type)0), res.length((int)0), find_variable(key));
		}
	}
}

void
xml_variable_map::add_variable(std::string const &name, std::string const &value) {
	std::map<std::string, std::string>::iterator i = vars_.find(name);
	if (vars_.end() != i) {
		throw error("duplicate variable %s", name.c_str());
	}
	vars_[name] = value;
}

std::string const&
xml_variable_map::find_variable(std::string const &name) const {
	std::map<std::string, std::string>::const_iterator i = vars_.find(name);
	if (vars_.end() == i) {
		throw error("nonexistent variable %s", name.c_str());
	}
	return i->second;
}

}} // namespaces
