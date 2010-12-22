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

#ifndef XIVA_PYTHON_PYTHON_SETTINGS_HPP_INCLUDED
#define XIVA_PYTHON_PYTHON_SETTINGS_HPP_INCLUDED

#include <string>
#include <boost/python.hpp>

#include "xiva/settings.hpp"

namespace py = boost::python;

namespace xiva { namespace python {

class python_settings : public settings {

public:
	python_settings(py::object const &impl);
	virtual ~python_settings();

	virtual std::string address() const;
	virtual unsigned short port() const;
	virtual unsigned short backlog() const;

	virtual std::string ssl_address() const;
	virtual unsigned short ssl_port() const;
	virtual unsigned short ssl_backlog() const;
	virtual std::string ssl_cert_file_name() const;
	virtual std::string ssl_cacert_file_name() const;

	virtual unsigned int read_timeout() const;
	virtual unsigned int write_timeout() const;
	virtual unsigned int inactive_timeout() const;

	virtual unsigned int ping_interval() const;

	virtual unsigned short handler_threads() const;
	virtual unsigned short listener_threads() const;

	virtual std::string policy_file_name() const;

	virtual std::string value(char const *name) const;
	virtual enumeration<std::string>::ptr_type value_list(char const *prefix) const;

private:
	python_settings(python_settings const &);
	python_settings& operator = (python_settings const &);

	template <typename Result> Result get(char const *method, Result const &defval) const;

private:
	py::object impl_;
};

}} // namespaces

#endif // XIVA_PYTHON_PYTHON_SETTINGS_HPP_INCLUDED
