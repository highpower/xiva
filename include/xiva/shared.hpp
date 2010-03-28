/** @file shared.hpp */
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

#ifndef XIVA_SHARED_HPP_INCLUDED
#define XIVA_SHARED_HPP_INCLUDED

#include <cassert>

#include "xiva/config.hpp"

namespace xiva {

class shared;

void intrusive_ptr_add_ref(shared *object);
void intrusive_ptr_release(shared *object);

class XIVA_API shared {

public:
	shared();
	virtual ~shared();

private:
	shared(const shared &);
	shared& operator = (const shared &);

	friend void intrusive_ptr_add_ref(shared *object);
	friend void intrusive_ptr_release(shared *object);

private:
	int count_;
};

inline
shared::shared() :
		count_(0) {
}

inline
shared::~shared() {
	assert(0 == count_);
}

inline XIVA_API void
intrusive_ptr_add_ref(shared *object) {
	++object->count_;
}


inline XIVA_API void
intrusive_ptr_release(shared *object) {
	if (0 == --object->count_) delete object;
}

} // namespace

#endif // XIVA_SHARED_HPP_INCLUDED
