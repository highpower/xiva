#include "acsetup.hpp"
#include "variable_map.hpp"

#include "xiva/error.hpp"

namespace xiva { namespace daemon {

template <typename Char>
struct var_constants;

template <>
struct var_constants<char> {
	static char const end_token[];
	static char const begin_token[];
};

char const
var_constants<char>::end_token[] = "}";

char const
var_constants<char>::begin_token[] = "${";

template <typename String, typename Map> String
substitute(String const &str, Map const &vars) {

	typedef typename String::value_type char_type;

	typename String::size_type begin = str.find(var_constants<char_type>::begin_token);
	if (String::npos == begin) {
		return str;
	}
	typename String::size_type end = str.rfind(var_constants<char_type>::end_token);
	if (String::npos == end) {
		throw error("bad variable syntax: %s", str.c_str());
	}

	typename String::size_type send = end - (sizeof(var_constants<char_type>::end_token) - 1);
	typename String::size_type sbegin = begin + (sizeof(var_constants<char_type>::begin_token) - 1);

	String copy(str);
	String result = substitute(String(str, sbegin, send - sbegin + 1), vars);

	typename Map::const_iterator i = vars.find(result);
	if (vars.end() == i) {
		throw error("nonexistent variable: %s", result.c_str());
	}

	copy.replace(begin, end - begin + 1, i->second);
	return copy;
}


variable_map::variable_map() {
}

variable_map::~variable_map() {
}

void
variable_map::resolve_variables(std::string &str) {
	str = substitute(str, vars_);
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

}} // namespaces
