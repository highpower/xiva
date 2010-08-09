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

#ifndef XIVA_CHANNEL_ID_HPP_INCLUDED
#define XIVA_CHANNEL_ID_HPP_INCLUDED


#include <string>

#include "xiva/config.hpp"

namespace xiva {

class XIVA_API channel_id {

public:
	channel_id();
	channel_id(std::string const &name, std::string const &key);

	// simple non-virtual destructor
	~channel_id();

	void assign(std::string const &name);
	void assign(std::string const &name, std::string const &key);


	bool operator < (channel_id const& other) const;

	bool empty() const;

	std::string const& name() const;
	std::string const& key() const;

private:
	std::string name_;
	std::string key_;
};


inline
channel_id::channel_id()
{
}

inline
channel_id::channel_id(std::string const &name, std::string const &key) :
	name_(name), key_(key)
{
}

inline
channel_id::~channel_id() {
}

inline void
channel_id::assign(std::string const &name) {
	name_.assign(name);
	key_.clear();
}

inline void
channel_id::assign(std::string const &name, std::string const &key) {
	name_.assign(name);
	key_.assign(key);
}

inline bool
channel_id::operator < (channel_id const& other) const {
	if (name_ > other.name_) {
		return false;
	}
	return name_ < other.name_ || key_ < other.key_;
}

inline bool
channel_id::empty() const {
	return name_.empty() && key_.empty();
}

inline std::string const&
channel_id::name() const {
	return name_;
}

inline std::string const&
channel_id::key() const {
	return key_;
}

} // namespace

#endif // XIVA_CHANNEL_ID_HPP_INCLUDED
