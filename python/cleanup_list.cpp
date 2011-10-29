#include "acsetup.hpp"
#include "cleanup_list.hpp"

#include <exception>
#include <cassert>

#include <boost/bind.hpp>

#include "interpreter_lock.hpp"

namespace xiva { namespace python {

cleanup_list::cleanup_list() : started_(false), stopped_(false)
{
}

cleanup_list::~cleanup_list() {
	try {
		stop();
		join_all();
	}
	catch (std::exception const &e) {
		(void) e;
	}
}

void
cleanup_list::start() {
	assert(!started_);
	started_ = true;
	boost::function<void()> f = boost::bind(&cleanup_list::thread_func, this);
	create_thread(f);
}

void
cleanup_list::stop() {
	boost::mutex::scoped_lock lock(mutex_);
	stopped_ = true;
	condition_.notify_all();
}

void
cleanup_list::push_ref(boost::shared_ptr<py::object> &obj_ref) {

	boost::shared_ptr<py::object> obj;
	obj.swap(obj_ref);

	boost::mutex::scoped_lock lock(mutex_);
	input_.push_back(obj);
	condition_.notify_all();
}

bool
cleanup_list::acquire_destroy_list() {
	boost::mutex::scoped_lock lock(mutex_);
	while (!stopped_ && input_.empty()) {
		condition_.wait(lock);
	}
	if (stopped_) {
		return false;
	}
	to_destroy_.swap(input_);
	return true;
}

void
cleanup_list::thread_func() {
	while (!stopped_) {
		try {
			if (!acquire_destroy_list()) {
				return;
			}

			interpreter_try_lock py_lock;
			if (stopped_ || !py_lock.acquired()) {
				return;
			}

			objects_type to_destroy_now;
			to_destroy_now.swap(to_destroy_);
		}
		catch (std::exception const &e) {
			(void) e;
		}
	}
}


cleanup_list_auto::cleanup_list_auto(cleanup_list &cleanup) : cleanup_(cleanup) {
	cleanup_.start();
}

cleanup_list_auto::~cleanup_list_auto() {
	cleanup_.stop();
}


}} // namespaces
