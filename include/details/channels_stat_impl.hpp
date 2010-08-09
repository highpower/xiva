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

#ifndef XIVA_DETAILS_CHANNELS_STAT_IMPL_HPP_INCLUDED
#define XIVA_DETAILS_CHANNELS_STAT_IMPL_HPP_INCLUDED

#include <string>
#include <map>

#include <boost/thread/mutex.hpp>
#include <boost/intrusive_ptr.hpp>

#include "xiva/forward.hpp"
#include "xiva/enumeration.hpp"

namespace xiva { namespace details {

class channels_stat_impl {

public:
	channels_stat_impl();
	virtual ~channels_stat_impl();

	void inc(channel_id const &ch_id);
	void dec(channel_id const &ch_id);

	enumeration<std::string>::ptr_type load_names() const;
	enumeration<std::string>::ptr_type load_keys(std::string const &ch_name) const;

private:
	channels_stat_impl(channels_stat_impl const &);
	channels_stat_impl& operator = (channels_stat_impl const &);

	typedef std::map<std::string, unsigned int> channel_stat;

	mutable boost::mutex mutex_;
	std::map<std::string, channel_stat> data_;
};

}} // namespaces

#endif // XIVA_DETAILS_CHANNELS_STAT_IMPL_HPP_INCLUDED
