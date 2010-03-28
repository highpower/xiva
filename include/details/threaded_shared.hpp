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

#ifndef XIVA_DETAILS_THREADED_SHARED_HPP_INCLUDED
#define XIVA_DETAILS_THREADED_SHARED_HPP_INCLUDED

#include <cassert>
#include <boost/thread/mutex.hpp>

namespace xiva { namespace details {

class threaded_shared;

void intrusive_ptr_add_ref(threaded_shared *object);
void intrusive_ptr_release(threaded_shared *object);

class threaded_shared {

public:
	threaded_shared();
	virtual ~threaded_shared();

private:
	threaded_shared(const threaded_shared &);
	threaded_shared& operator = (const threaded_shared &);

	friend void intrusive_ptr_add_ref(threaded_shared *object);
	friend void intrusive_ptr_release(threaded_shared *object);

protected:
	mutable boost::mutex mutex_;

private:
	int count_;
};

inline
threaded_shared::threaded_shared() :
	count_(0)
{
}

inline
threaded_shared::~threaded_shared() {
	assert(0 == count_);
}

inline void
intrusive_ptr_add_ref(threaded_shared *object) {
	boost::mutex::scoped_lock lock(object->mutex_);
	++object->count_;
}


inline void
intrusive_ptr_release(threaded_shared *object) {
	bool last = false;
	{
		boost::mutex::scoped_lock lock(object->mutex_);
		last = (0 == --object->count_);
	}
	if (last) {
		delete object;
	}
}

}} // namespaces

#endif // XIVA_DETAILS_THREADED_SHARED_HPP_INCLUDED
