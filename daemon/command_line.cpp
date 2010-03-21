#include "acsetup.hpp"
#include "command_line.hpp"

#include <cstring>
#include <cstdlib>
#include <ostream>

#include "xiva/error.hpp"

namespace xiva { namespace daemon {

command_line::command_line(int argc, char *argv[]) :
	help_mode_(false)
{
	for (std::size_t i = 1; i < static_cast<std::size_t>(argc); ++i) {
		if (strncmp("--help", argv[i], sizeof("--help")) == 0) {
			help_mode_ = true;
			break;
		}
		else if (strncmp("--config=", argv[i], sizeof("--config=") - 1) == 0) {
			config_file_.assign(argv[i] + sizeof("--config=") - 1);
		}
		else if (strncmp("--dump-variable=", argv[i], sizeof("--dump-variable=") - 1) == 0) {
			dump_variable_.assign(argv[i] + sizeof("--dump-variable=") - 1);
		}
	}
}

command_line::~command_line() {
}

bool
command_line::is_help_mode() const {
	return help_mode_;
}

bool
command_line::is_dump_mode() const {
	return !dump_variable_.empty();
}

bool
command_line::is_config_file_set() const {
	return !config_file_.empty();
}

std::string const&
command_line::config_file() const {
	return config_file_;
}

std::string const&
command_line::dump_variable() const {
	return dump_variable_;
}

std::ostream&
command_line::print_usage(std::ostream &stream) {
	stream << "usage: xiva-daemon --help | --config=<file> [--dump-variable=<var>]" << std::endl;
	return stream;
}

}} // namespaces
