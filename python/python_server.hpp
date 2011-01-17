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
#include <boost/intrusive_ptr.hpp>

#include "xiva/forward.hpp"
//#include "details/dynamic_loader.hpp"

#include "cleanup_list.hpp"

namespace py = boost::python;

namespace xiva { namespace python {

class python_logger;

class python_server {

public:
	python_server();
	virtual ~python_server();

	void stop();
	void init(py::object const &impl);
	void list_channels_enable();
	void start();

	py::list list_channels() const;
	py::list list_channel(std::string const &channel_name) const;

	void send(std::string const &to, std::string const &msg);
	void send_to_channel(std::string const &to, std::string const &msg,
		std::string const &channel_name, std::string const &channel_key, std::string const &channel_data);

	//void load(std::string const &name);

	void attach_logger(py::object const &impl);
	void attach_response_handler(py::object const &impl);
	void attach_ping_formatter(py::object const &impl);
	void attach_formatter_creator(std::string const &fmt_id, py::object const &impl);
	void add_connection_listener(py::object const &impl);
	void attach_message_filter(py::object const &impl);

private:
	python_server(python_server const &);
	python_server& operator = (python_server const &);

	void check_server() const;

private:
	//details::dynamic_loader loader_;
	cleanup_list cleanup_;
	boost::shared_ptr<details::server_impl> impl_;
	boost::intrusive_ptr<python_logger> logger_;
	boost::shared_ptr<details::channels_stat_impl> channels_stat_;
};

}} // namespaces

#endif // XIVA_PYTHON_PYTHON_SERVER_HPP_INCLUDED
