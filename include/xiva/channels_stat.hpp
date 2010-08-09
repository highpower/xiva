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

#ifndef XIVA_CHANNELS_STAT_HPP_INCLUDED
#define XIVA_CHANNELS_STAT_HPP_INCLUDED

#include <string>

#include <boost/shared_ptr.hpp>

#include "xiva/forward.hpp"
#include "xiva/enumeration.hpp"

namespace xiva {

class channels_stat {

public:
	channels_stat(boost::shared_ptr<details::channels_stat_impl> const &impl);
	virtual ~channels_stat();

	enumeration<std::string>::ptr_type load_names() const;
	enumeration<std::string>::ptr_type load_keys(std::string const &ch_name) const;

private:
	channels_stat(channels_stat const &);
	channels_stat& operator = (channels_stat const &);

	boost::shared_ptr<details::channels_stat_impl> impl_;
};

} // namespace

#endif // XIVA_CHANNELS_STAT_HPP_INCLUDED
