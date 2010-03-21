/** @file python_matcher.hpp */
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

#ifndef XIVA_PYTHON_PYTHON_MATCHER_HPP_INCLUDED
#define XIVA_PYTHON_PYTHON_MATCHER_HPP_INCLUDED

#include <boost/python.hpp>

#include "xiva/receiver_matcher.hpp"

namespace py = boost::python;

namespace xiva { namespace python {

class python_matcher : public receiver_matcher {

public:
	python_matcher(py::object const &impl);
	virtual ~python_matcher();
	
	virtual bool threaded() const;
	virtual char const* content_type() const;
	virtual std::string receiver(request const &req) const;

private:
	python_matcher(python_matcher const &);
	python_matcher& operator = (python_matcher const &);
	
private:
	py::object impl_;
};

}} // namespaces

#endif // XIVA_PYTHON_PYTHON_MATCHER_HPP_INCLUDED
