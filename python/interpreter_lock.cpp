#include "acsetup.hpp"
#include "gil.hpp"

#include <Python.h>

namespace xiva { namespace python {

static void
lock_interpreter() {
}

static void
unlock_interpreter() {
}


interpreter_lock::interpreter_lock()
{
	lock_interpreter();
}

interpreter_lock::~interpreter_lock() {
	unlock_interpreter();
}

interpreter_unlock::interpreter_unlock()
{
	unlock_interpreter();
}

interpreter_unlock::~interpreter_unlock() {
	lock_interpreter();
}

}} // namespaces
