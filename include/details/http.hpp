/** @file http.hpp */
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

#ifndef XIVA_DETAILS_HTTP_HPP_INCLUDED
#define XIVA_DETAILS_HTTP_HPP_INCLUDED

#include <iosfwd>
#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace xiva { namespace details {

class http_date;
std::ostream& operator << (std::ostream &stream, http_date const &val);

class http_date {

public:
	http_date(boost::posix_time::ptime const &p);
	friend std::ostream& operator << (std::ostream &stream, http_date const &val);

private:
	boost::posix_time::ptime when_;
};

class http_header;
std::ostream& operator << (std::ostream &stream, http_header const &val);

class http_header {

public:
	http_header(char const *name, char const *value);
	friend std::ostream& operator << (std::ostream &stream, http_header const &val);

	static http_header server();
	static http_header connection_close();
	static http_header connection_keep_alive();
	
private:
	char const *name_, *value_;
};

class http_status;
std::ostream& operator << (std::ostream &stream, http_status const &val);

class http_status {

public:
	http_status(unsigned short code);
	friend std::ostream& operator << (std::ostream &stream, http_status const &val);
	static char const* str(unsigned short code);

private:
	unsigned short code_;
};

}} // namespaces

#endif // XIVA_DETAILS_HTTP_HPP_INCLUDED
