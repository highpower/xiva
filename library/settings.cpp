#include "acsetup.hpp"
#include "xiva/settings.hpp"

namespace xiva {

settings::settings()
{
}

settings::~settings() {
}

std::string
settings::value(std::string const &name) const {
	return value(name.c_str());
}

enumeration<std::string>::ptr_type
settings::value_list(std::string const &prefix) const {
	return value_list(prefix.c_str());
}

} // namespace
