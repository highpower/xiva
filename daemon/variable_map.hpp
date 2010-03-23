/** @file variable_map.hpp */
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

#ifndef XIVA_DAEMON_VARIABLE_MAP_HPP_INCLUDED
#define XIVA_DAEMON_VARIABLE_MAP_HPP_INCLUDED

#include <map>
#include <string>

#include "xiva/reference_counted.hpp"

namespace xiva { namespace daemon {

class variable_map : public reference_counted {

public:
	variable_map();
	virtual ~variable_map();
	
	void resolve_variables(std::string &str);
	void add_variable(std::string const &name, std::string const &value);
	
private:
	variable_map(variable_map const &);
	variable_map& operator = (variable_map const &);
	
	std::string const& find_variable(std::string const &name) const;
	void resolve_variables(std::string &str, std::string::size_type begin, std::string::size_type end);
	
private:
	std::map<std::string, std::string> vars_;
};

}} // namespaces

#endif // XIVA_DAEMON_VARIABLE_MAP_HPP_INCLUDED
