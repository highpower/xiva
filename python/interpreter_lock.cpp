#include "acsetup.hpp"
#include "interpreter_lock.hpp"

#include <cassert>
#include <stdexcept>

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

#include <Python.h>

namespace xiva { namespace python {

class interpreter_lock_impl {

public:
	interpreter_lock_impl();
	virtual ~interpreter_lock_impl();

	void lock_thread();
	void unlock_thread();

	bool lock();
	void unlock();

private:
	interpreter_lock_impl(interpreter_lock_impl const &);
	interpreter_lock_impl& operator = (interpreter_lock_impl const &);

	boost::mutex mutex_;
	boost::condition condition_;
	PyThreadState *main_thread_state_;
	PyThreadState *thread_state_;
	int thread_lock_count_;
};

int interpreter_init::count_ = 0;

//std::auto_ptr does not work because it zeroize ptr after interpreter_init()
static interpreter_lock_impl *interpreter_impl_ = NULL;


interpreter_init::interpreter_init() {
	if (0 == count_++) {
		assert(NULL == interpreter_impl_);
		interpreter_impl_ = new interpreter_lock_impl();
	}
}

interpreter_init::~interpreter_init() {
	if (!--count_) {
		delete interpreter_impl_;
	}
}

interpreter_thread_lock::interpreter_thread_lock() {
	assert(NULL != interpreter_impl_);
	interpreter_impl_->lock_thread();
}

interpreter_thread_lock::~interpreter_thread_lock() {
	assert(NULL != interpreter_impl_);
	interpreter_impl_->unlock_thread();
}


interpreter_lock::interpreter_lock() {
	assert(NULL != interpreter_impl_);
	if (!interpreter_impl_->lock()) {
		throw std::runtime_error("Can not acquire python interpreter lock");
	}
}

interpreter_lock::~interpreter_lock() {
	assert(NULL != interpreter_impl_);
	interpreter_impl_->unlock();
}

interpreter_try_lock::interpreter_try_lock() {
	assert(NULL != interpreter_impl_);
	acquired_ = interpreter_impl_->lock();
}

interpreter_try_lock::~interpreter_try_lock() {
	assert(NULL != interpreter_impl_);
	if (acquired_) {
		interpreter_impl_->unlock();
	}
}

interpreter_unlock::interpreter_unlock() : save_(PyEval_SaveThread())
{
}

interpreter_unlock::~interpreter_unlock() {
	PyEval_RestoreThread(reinterpret_cast<PyThreadState*>(save_));
}


interpreter_lock_impl::interpreter_lock_impl() : main_thread_state_(NULL), thread_state_(NULL), thread_lock_count_(-1) {
	PyEval_InitThreads();
	main_thread_state_ = PyThreadState_Get();
	assert(NULL != main_thread_state_);
}

interpreter_lock_impl::~interpreter_lock_impl() {
}

void
interpreter_lock_impl::lock_thread() {
	boost::mutex::scoped_lock lock(mutex_);
	assert(NULL == thread_state_);
	thread_state_ = PyEval_SaveThread();
	assert(NULL != thread_state_);
	assert(thread_lock_count_ < 0);
	thread_lock_count_ = 0;
	condition_.notify_all();
}

void
interpreter_lock_impl::unlock_thread() {
	boost::mutex::scoped_lock lock(mutex_);
	while (thread_lock_count_ > 0) {
		condition_.wait(lock);
	}
	assert(NULL != thread_state_);
	PyEval_RestoreThread(thread_state_);
	thread_lock_count_ = -1;
	thread_state_ = NULL;
	condition_.notify_all();
}

bool
interpreter_lock_impl::lock() {
	boost::mutex::scoped_lock lock(mutex_);
	while (NULL != thread_state_) {
		if (!thread_lock_count_) {
			break;
		}
		condition_.wait(lock);
	}
	if (NULL == thread_state_) {
		return false;
	}
	PyEval_RestoreThread(thread_state_);
	thread_lock_count_++;
	return true;
}

void
interpreter_lock_impl::unlock() {
	boost::mutex::scoped_lock lock(mutex_);
	assert(NULL != thread_state_);
	/* thread_state_ = */ PyEval_SaveThread();
	if (!--thread_lock_count_) {
		condition_.notify_all();
	}
}

}} // namespaces
