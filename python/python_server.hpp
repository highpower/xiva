/** @file python_server.hpp */
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

#ifndef XIVA_PYTHON_PYTHON_SERVER_HPP_INCLUDED
#define XIVA_PYTHON_PYTHON_SERVER_HPP_INCLUDED

#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>

#include "xiva/forward.hpp"
#include "details/dynamic_loader.hpp"

namespace py = boost::python;

namespace xiva { namespace python {

class python_server {

public:
	python_server();
	virtual ~python_server();

	void stop();
	void start(py::object const &impl);
	
	void load(std::string const &name);
	void attach_logger(py::object const &impl);
	void attach_receiver_matcher(py::object const &impl);
	void add_connection_listener(py::object const &impl);

private:
	python_server(python_server const &);
	python_server& operator = (python_server const &);

private:
	details::dynamic_loader loader_;
	boost::shared_ptr<details::server_impl> impl_;
};

}} // namespaces

#endif // XIVA_PYTHON_PYTHON_SERVER_HPP_INCLUDED
