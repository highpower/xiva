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
#include <utility>

#include <boost/python.hpp>
#include <boost/thread/thread.hpp>

#include "xiva/logger.hpp"

#include "details/threaded_queue.hpp"

namespace py = boost::python;

namespace xiva { namespace python {

class python_logger : public logger, private boost::thread_group {

public:
	explicit python_logger(py::object const &impl);
	virtual ~python_logger();

	void start();
	void thread_func();

	virtual void info(char const *format, ...);
	virtual void debug(char const *format, ...);
	virtual void error(char const *format, ...);

private:
	python_logger(python_logger const &);
	python_logger& operator = (python_logger const &);

	typedef std::pair<char, std::string> queue_item_type;
	typedef details::threaded_queue<queue_item_type> queue_type;
	typedef queue_type::raw_items_type items_type;

	void process_items(items_type const &items) const;
	void invoke(char level, char const *format, va_list args);

	py::object impl_;
	queue_type items_;

	bool enabled_debug_;
	bool enabled_info_;
	bool enabled_error_;
};

}} // namespaces

#endif // XIVA_PYTHON_PYTHON_LOGGER_HPP_INCLUDED
