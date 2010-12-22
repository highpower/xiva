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

#ifndef XIVA_PYTHON_CLEANUP_LIST_HPP_INCLUDED
#define XIVA_PYTHON_CLEANUP_LIST_HPP_INCLUDED

#include <deque>

#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/thread.hpp>

namespace py = boost::python;

namespace xiva { namespace python {

class cleanup_list : private boost::thread_group {

public:
	cleanup_list();
	virtual ~cleanup_list();

	void start();
	void stop();

	void push_ref(boost::shared_ptr<py::object> &obj_ref);

private:
	bool acquire_destroy_list();
	void thread_func();

private:
	cleanup_list(cleanup_list const &);
	cleanup_list& operator = (cleanup_list const &);

	typedef std::deque< boost::shared_ptr<py::object> > objects_type;

	boost::mutex mutex_;
	boost::condition condition_;
	objects_type input_;
	objects_type to_destroy_;
	bool started_;
	volatile bool stopped_;
};

class cleanup_list_auto {
public:
	explicit cleanup_list_auto(cleanup_list &cleanup);
	~cleanup_list_auto();

private:
	cleanup_list_auto(cleanup_list_auto const &);
	cleanup_list_auto& operator = (cleanup_list_auto const &);

	cleanup_list &cleanup_;
};

}} // namespaces

#endif // XIVA_PYTHON_CLEANUP_LIST_HPP_INCLUDED
