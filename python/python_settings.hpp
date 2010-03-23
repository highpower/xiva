/** @file python_settings.hpp */
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
#include <stdexcept>
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

	virtual unsigned short read_timeout() const;
	virtual unsigned short write_timeout() const;
	virtual unsigned int inactive_timeout() const;

	virtual unsigned short listener_threads() const;
	virtual unsigned short matcher_threads() const;

	virtual std::string policy_file_name() const;

	virtual std::string value(char const *name) const;
	virtual enumeration<std::string>::ptr_type value_list(char const *prefix) const;
	
private:
	python_settings(python_settings const &);
	python_settings& operator = (python_settings const &);
	template <typename Result> void init_if_exists(py::object const &impl, char const *method, Result &res) const;

private:
	std::string address_, policy_file_name_;
	unsigned short port_, backlog_, read_timeout_, write_timeout_;
	unsigned int inactive_timeout_;
	unsigned short listener_threads_, matcher_threads_;
};

template <typename Result> inline void
python_settings::init_if_exists(py::object const &impl, char const *method, Result &res) const {
	if (!impl) {
		return;
	}
	py::object func = impl.attr(method);
	if (func) {
		res = py::call<Result>(func.ptr());
	}
}

}} // namespaces

#endif // XIVA_PYTHON_PYTHON_SETTINGS_HPP_INCLUDED
