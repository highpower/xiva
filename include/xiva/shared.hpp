/** @file reference_counted.hpp */
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

#ifndef XIVA_REFERENCE_COUNTED_HPP_INCLUDED
#define XIVA_REFERENCE_COUNTED_HPP_INCLUDED

#include <cassert>

#include "xiva/config.hpp"

namespace xiva {

class reference_counted;

void intrusive_ptr_add_ref(reference_counted *object); 
void intrusive_ptr_release(reference_counted *object); 

class XIVA_API reference_counted {

public:
	reference_counted();
	virtual ~reference_counted();

private:
	reference_counted(const reference_counted &);
	reference_counted& operator = (const reference_counted &);

	friend void intrusive_ptr_add_ref(reference_counted *object);
	friend void intrusive_ptr_release(reference_counted *object);

private:
	int count_;
};

inline
reference_counted::reference_counted() : 
	count_(0)
{
}

inline 
reference_counted::~reference_counted() {
	assert(0 == count_);
}

inline XIVA_API void
intrusive_ptr_add_ref(reference_counted *object) {
	++object->count_;
}


inline XIVA_API void
intrusive_ptr_release(reference_counted *object) {
	if (0 == --object->count_) delete object;
}

} // namespace

#endif // XIVA_REFERENCE_COUNTED_HPP_INCLUDED
