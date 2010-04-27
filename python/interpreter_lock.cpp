#include "acsetup.hpp"
#include "interpreter_lock.hpp"

#include <Python.h>

#include <cassert>


namespace xiva { namespace python {

int
interpreter_init::count_ = 0;


static PyThreadState *save_state_ = NULL;
static PyThreadState *main_thread_state_ = NULL;

interpreter_init::interpreter_init() {
	if (0 == count_++) {
		PyEval_InitThreads();
		main_thread_state_ = PyThreadState_Get();
	}
}

interpreter_init::~interpreter_init() {
	--count_;
}


interpreter_thread_lock::interpreter_thread_lock() {
	assert(NULL == save_state_);
	save_state_ = PyThreadState_Swap(NULL);
	PyEval_ReleaseLock();
}

interpreter_thread_lock::~interpreter_thread_lock() {
	PyEval_AcquireLock();
	if (save_state_) {
		PyThreadState *save_state = save_state_;
    		save_state_ = NULL;
		PyThreadState_Swap(save_state);
	}
	else {
		PyThreadState_Swap(main_thread_state_);
	}
}


interpreter_lock::interpreter_lock() : threaded_(false) {
	PyEval_AcquireLock();
	if (NULL != save_state_) {
		threaded_ = true;
		PyThreadState *save_state = save_state_;
		save_state_ = NULL;
		PyThreadState_Swap(save_state);
	}
}

interpreter_lock::~interpreter_lock() {
	if (threaded_) {
		save_state_ = PyThreadState_Swap(NULL);
	}
	PyEval_ReleaseLock();
}

interpreter_unlock::interpreter_unlock() {
	PyEval_ReleaseLock();
}

interpreter_unlock::~interpreter_unlock() {
	PyEval_AcquireLock();
}

}} // namespaces
