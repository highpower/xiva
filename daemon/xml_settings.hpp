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

#ifndef XIVA_DAEMON_XML_SETTINGS_HPP_INCLUDED
#define XIVA_DAEMON_XML_SETTINGS_HPP_INCLUDED

#include <boost/intrusive_ptr.hpp>

#include "xml.hpp"
#include "xiva/settings.hpp"

namespace xiva { namespace daemon {

class variable_map;

class xml_settings : public settings {

public:
	xml_settings(char const *file);
	xml_settings(int argc, char *argv[]);
	virtual ~xml_settings();

	virtual std::string address() const;
	virtual unsigned short port() const;
	virtual unsigned short backlog() const;

	virtual unsigned int read_timeout() const;
	virtual unsigned int write_timeout() const;
	virtual unsigned int inactive_timeout() const;

	virtual unsigned short handler_threads() const;
	virtual unsigned short listener_threads() const;

	virtual std::string policy_file_name() const;

	virtual std::string value(char const *name) const;
	virtual enumeration<std::string>::ptr_type value_list(char const *prefix) const;

private:
	xml_settings(xml_settings const &);
	xml_settings& operator = (xml_settings const &);

	void find_variables();
	void init(char const *name);

private:
	xml::doc doc_;
	boost::intrusive_ptr<variable_map> vars_;
};

}} // namespaces

#endif // XIVA_DAEMON_XML_SETTINGS_HPP_INCLUDED
