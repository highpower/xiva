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

#ifndef XIVA_PYTHON_PYTHON_MESSAGE_FILTER_HPP_INCLUDED
#define XIVA_PYTHON_PYTHON_MESSAGE_FILTER_HPP_INCLUDED

#include <boost/python.hpp>

#include "xiva/forward.hpp"
#include "xiva/message_filter.hpp"

namespace py = boost::python;

namespace xiva { namespace python {

class python_message_filter : public xiva::message_filter {

public:
	python_message_filter(py::object const &impl);
	virtual ~python_message_filter();

	virtual bool allow_message(message const &msg, std::string const &channel_data, formatter const *fmt) const;

private:
	python_message_filter(python_message_filter const &);
	python_message_filter& operator = (python_message_filter const &);

	py::object impl_;
};

}} // namespaces

#endif // XIVA_PYTHON_PYTHON_MESSAGE_FILTER_HPP_INCLUDED
