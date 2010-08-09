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

#ifndef XIVA_CHANNEL_INFO_HPP_INCLUDED
#define XIVA_CHANNEL_INFO_HPP_INCLUDED


#include <string>

#include "xiva/config.hpp"
#include "xiva/channel_id.hpp"

namespace xiva {

class XIVA_API channel_info : public channel_id {

public:
	channel_info();
	channel_info(std::string const &name, std::string const &key, std::string const &data);

	// simple non-virtual destructor
	~channel_info();

	std::string const& data() const;
	void data(std::string const &value);

private:
	std::string data_;
};


inline
channel_info::channel_info()
{
}

inline
channel_info::channel_info(std::string const &name, std::string const &key, std::string const &data) :
	channel_id(name, key), data_(data)
{
}

inline
channel_info::~channel_info() {
}

inline std::string const&
channel_info::data() const {
	return data_;
}

inline void
channel_info::data(std::string const &value) {
	data_.assign(value);
}


} // namespace

#endif // XIVA_CHANNEL_INFO_HPP_INCLUDED
