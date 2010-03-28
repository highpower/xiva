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

#ifndef XIVA_PYTHON_PYTHON_LOGGER_HPP_INCLUDED
#define XIVA_PYTHON_PYTHON_LOGGER_HPP_INCLUDED

#include <cstdarg>
#include <boost/python.hpp>

#include "xiva/logger.hpp"

namespace py = boost::python;

namespace xiva { namespace python {

class python_logger : public logger {

public:
	python_logger(py::object const &impl);
	virtual ~python_logger();

	virtual void info(char const *format, ...);
	virtual void debug(char const *format, ...);
	virtual void error(char const *format, ...);

private:
	python_logger(python_logger const &);
	python_logger& operator = (python_logger const &);
	void invoke(char const *method, char const *format, va_list args);

private:
	py::object impl_;
};

}} // namespaces

#endif // XIVA_PYTHON_PYTHON_LOGGER_HPP_INCLUDED
