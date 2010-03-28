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

#ifndef XIVA_PYTHON_PYTHON_LISTENER_HPP_INCLUDED
#define XIVA_PYTHON_PYTHON_LISTENER_HPP_INCLUDED

#include <boost/python.hpp>

#include "xiva/connection_listener.hpp"

namespace py = boost::python;

namespace xiva { namespace python {

class python_listener : public connection_listener {

public:
	python_listener(py::object const &impl);
	virtual ~python_listener();

	virtual void connection_opened(std::string const &to, globals::connection_id const &id) throw (std::exception);
	virtual void connection_closed(std::string const &to, globals::connection_id const &id) throw (std::exception);

private:
	python_listener(python_listener const &);
	python_listener& operator = (python_listener const &);

private:
	py::object impl_;
};

}} // namespaces

#endif // XIVA_PYTHON_PYTHON_LISTENER_HPP_INCLUDED
