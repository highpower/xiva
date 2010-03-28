// xiva (acronym for HTTP Extended EVent Automata) is a simple HTTP server.
// Copyright (C) 2009 Yandex <highpower@yandex.ru>

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef XIVA_DAEMON_COMMAND_LINE_HPP_INCLUDED
#define XIVA_DAEMON_COMMAND_LINE_HPP_INCLUDED

#include <iosfwd>
#include <string>

namespace xiva { namespace daemon {

class command_line {

public:
	command_line(int argc, char *argv[]);
	virtual ~command_line();

	bool is_help_mode() const;
	bool is_dump_mode() const;
	bool is_config_file_set() const;

	std::string const& config_file() const;
	std::string const& dump_variable() const;

	static std::ostream& print_usage(std::ostream &stream);

private:
	bool help_mode_;
	std::string config_file_, dump_variable_;
};

}} // namespaces

#endif // XIVA_DAEMON_COMMAND_LINE_HPP_INCLUDED
