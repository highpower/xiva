/** @file settings.hpp */
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

#ifndef XIVA_SETTINGS_HPP_INCLUDED
#define XIVA_SETTINGS_HPP_INCLUDED

#include <string>
#include <boost/lexical_cast.hpp>

#include "xiva/config.hpp"
#include "xiva/enumeration.hpp"

namespace xiva {

class XIVA_API settings {

public:
	settings();
	virtual ~settings();

	virtual std::string address() const = 0;
	virtual unsigned short port() const = 0;
	virtual unsigned short backlog() const = 0;

	virtual unsigned short read_timeout() const = 0;
	virtual unsigned short write_timeout() const = 0;
	virtual unsigned int inactive_timeout() const = 0;

	virtual unsigned short listener_threads() const = 0;
	virtual unsigned short matcher_threads() const = 0;

	virtual std::string policy_file_name() const = 0;

	virtual std::string value(char const *name) const = 0;
	virtual enumeration<std::string>::ptr_type value_list(char const *prefix) const = 0;
	
	std::string value(std::string const &name) const;
	enumeration<std::string>::ptr_type value_list(std::string const &prefix) const;
	
	template <typename X> X as(char const *name) const;
	template <typename X> X as(std::string const &name) const;

private:
	settings(settings const &);
	settings& operator = (settings const &);
}; 	

template <typename X> inline X
settings::as(char const *name) const {
	return boost::lexical_cast<X>(value(name));
}

template <typename X> inline X
settings::as(std::string const &name) const {
	return boost::lexical_cast<X>(value(name));
}

} // namespace

#endif // XIVA_SETTINGS_HPP_INCLUDED
