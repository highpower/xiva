#include "acsetup.hpp"
#include "details/guard.hpp"

namespace xiva { namespace details {

static const int GUARD_NONE = 0;
static const int GUARD_OPENED = 1;
static const int GUARD_OK = 2;
static const int GUARD_CANCELED = 3;
static const int GUARD_CLOSED = 4;



guard::guard() : state_(GUARD_NONE)
{
}

guard::~guard() {
}

bool
guard::open() {
	boost::mutex::scoped_lock sl(mutex_);
	if (GUARD_NONE != state_) {
		return false;
	}
	state_ = GUARD_OPENED;
	return true;
}

bool
guard::commit() {
	boost::mutex::scoped_lock sl(mutex_);
	if (GUARD_OPENED != state_) {
		return false;
	}
	state_ = GUARD_OK;
	return true;
}

bool
guard::cancel() {
	boost::mutex::scoped_lock sl(mutex_);
	if (GUARD_OPENED != state_) {
		return false;
	}
	state_ = GUARD_CANCELED;
	return true;
}

bool
guard::close() {
	boost::mutex::scoped_lock sl(mutex_);
	if (GUARD_OK != state_) {
		return false;
	}
	state_ = GUARD_CLOSED;
	return true;
}
				
}} // namespaces
