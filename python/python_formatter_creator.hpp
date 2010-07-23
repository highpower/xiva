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

#ifndef XIVA_PYTHON_PYTHON_FORMATTER_CREATOR_HPP_INCLUDED
#define XIVA_PYTHON_PYTHON_FORMATTER_CREATOR_HPP_INCLUDED

#include <boost/python.hpp>

#include "xiva/forward.hpp"
#include "xiva/formatter.hpp"

namespace py = boost::python;

namespace xiva { namespace python {

class python_formatter_creator : public xiva::formatter_creator {

public:
	python_formatter_creator(py::object const &impl);
	virtual ~python_formatter_creator();

	virtual std::auto_ptr<formatter> create(request const &req) const;

private:
	python_formatter_creator(python_formatter_creator const &);
	python_formatter_creator& operator = (python_formatter_creator const &);

	py::object impl_;
};

}} // namespaces

#endif // XIVA_PYTHON_PYTHON_FORMATTER_CREATOR_HPP_INCLUDED
