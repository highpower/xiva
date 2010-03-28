#include "acsetup.hpp"
#include "interpreter_lock.hpp"

#include <Python.h>

namespace xiva { namespace python {

int
interpreter_init::count_ = 0;

interpreter_init::interpreter_init() {
	if (0 == count_++) {
		PyEval_InitThreads();
	}
}

interpreter_init::~interpreter_init() {
	--count_;
}

interpreter_lock::interpreter_lock() {
	PyEval_AcquireLock();
}

interpreter_lock::~interpreter_lock() {
	PyEval_ReleaseLock();
}

interpreter_unlock::interpreter_unlock() {
	PyEval_ReleaseLock();
}

interpreter_unlock::~interpreter_unlock() {
	PyEval_AcquireLock();
}

}} // namespaces
