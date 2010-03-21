/** @file connection_data.hpp */
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

#ifndef XIVA_DETAILS_CONNECTION_DATA_HPP_INCLUDED
#define XIVA_DETAILS_CONNECTION_DATA_HPP_INCLUDED

#include <boost/intrusive_ptr.hpp>

#include "xiva/forward.hpp"

namespace xiva { namespace details {

//class connection_manager;

class connection_data {

public:
	connection_data();
	virtual ~connection_data();

	void init(settings const &s);
	unsigned short read_timeout() const;
	unsigned short write_timeout() const;
	unsigned int inactive_timeout() const;
	unsigned int validate_timeout() const;

	boost::intrusive_ptr<receiver_matcher> const& matcher() const;
	void matcher(boost::intrusive_ptr<receiver_matcher> const &m);

	//boost::intrusive_ptr<connection_manager> const& manager() const;
	//void manager(boost::intrusive_ptr<connection_manager> const &m);

private:
	unsigned short read_timeout_, write_timeout_;
	unsigned int inactive_timeout_, validate_timeout_;
	boost::intrusive_ptr<receiver_matcher> matcher_;
	//boost::intrusive_ptr<connection_manager> manager_;
};

inline unsigned short
connection_data::read_timeout() const {
	return read_timeout_;
}

inline unsigned short
connection_data::write_timeout() const {
	return write_timeout_;
}

inline unsigned int
connection_data::inactive_timeout() const {
	return inactive_timeout_;
}

inline unsigned int
connection_data::validate_timeout() const {
	return validate_timeout_;
}

}} // namespaces

#endif // XIVA_DETAILS_CONNECTION_DATA_HPP_INCLUDED
