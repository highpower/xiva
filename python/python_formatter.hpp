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

#ifndef XIVA_PYTHON_PYTHON_FORMATTER_HPP_INCLUDED
#define XIVA_PYTHON_PYTHON_FORMATTER_HPP_INCLUDED

#include <boost/python.hpp>

#include "xiva/forward.hpp"
#include "xiva/formatter.hpp"

namespace py = boost::python;

namespace xiva { namespace python {

class python_formatter : public xiva::formatter {

public:
	python_formatter(py::object const &impl);
	virtual ~python_formatter();

	virtual std::string wrap_message(std::string const &content) const;
        virtual std::string ping_message() const;

private:
	python_formatter(python_formatter const &);
	python_formatter& operator = (python_formatter const &);

	py::object impl_;
};

}} // namespaces

#endif // XIVA_PYTHON_PYTHON_FORMATTER_HPP_INCLUDED
