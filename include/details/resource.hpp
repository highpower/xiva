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

#ifndef XIVA_DETAILS_RESOURCE_HPP_INCLUDED
#define XIVA_DETAILS_RESOURCE_HPP_INCLUDED

#include <algorithm>

namespace xiva { namespace details {

template <typename Base, typename Type>
struct pointer_access {
};

template <typename Base, typename Type>
struct pointer_access<Base, Type*> {
	Type* operator -> () const;
};

template <typename Type, typename Traits>
class resource : public pointer_access<resource<Type, Traits>, Type> {

public:
	resource();
	explicit resource(Type value, Traits const &traits = Traits());
	virtual ~resource();

	Type release();
	void reset(Type value);
	void swap(resource<Type, Traits> &other);
	Type get() const;

	class bool_convertible;
	operator bool_convertible const* () const;

private:
	resource(resource const &);
	resource& operator = (resource const &);

private:
	Type value_;
	Traits traits_;
};

template <typename Type>
struct default_traits {
	static Type default_value();
};

template <typename Base, typename Type> inline Type*
pointer_access<Base, Type*>::operator -> () const {
	return static_cast<Base const*>(this)->get();
};


template <typename Type, typename Traits> inline
resource<Type, Traits>::resource() :
	value_(Traits::default_value()), traits_()
{
}

template <typename Type, typename Traits> inline
resource<Type, Traits>::resource(Type value, Traits const &traits) :
	value_(value), traits_(traits)
{
}

template <typename Type, typename Traits> inline
resource<Type, Traits>::~resource() {
	if (Traits::default_value() != value_) {
		traits_.destroy(value_);
	}
}

template <typename Type, typename Traits> inline Type
resource<Type, Traits>::release() {
	Type result = Traits::default_value();
	std::swap(value_, result);
	return result;
}

template <typename Type, typename Traits> inline void
resource<Type, Traits>::reset(Type value) {
	resource<Type, Traits> tmp(value);
	swap(tmp);
}

template <typename Type, typename Traits> inline void
resource<Type, Traits>::swap(resource<Type, Traits> &other) {
	std::swap(value_, other.value_);
}

template <typename Type, typename Traits> inline Type
resource<Type, Traits>::get() const {
	return value_;
}

template <typename Type, typename Traits> inline
resource<Type, Traits>::operator typename resource<Type, Traits>::bool_convertible const* () const {
	return Traits::default_value() != value_ ? reinterpret_cast<bool_convertible const*>(this) : 0;
}

template <typename Type> inline Type
default_traits<Type>::default_value() {
	return static_cast<Type>(0);
}

}} // namespaces

#endif // XIVA_DETAILS_RESOURCE_HPP_INCLUDED
