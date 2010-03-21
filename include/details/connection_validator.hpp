/** @file connection_validator.hpp */
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

#ifndef XIVA_DETAILS_CONNECTION_VALIDATOR_HPP_INCLUDED
#define XIVA_DETAILS_CONNECTION_VALIDATOR_HPP_INCLUDED

#include <string>
#include <exception>

#include <boost/intrusive_ptr.hpp>

#include "xiva/forward.hpp"
#include "xiva/receiver_matcher.hpp"
#include "xiva/reference_counted.hpp"
#include "xiva/logger.hpp"

#include "details/connection.hpp"


namespace xiva { namespace details {

class request_impl;
class connection_data;


class connection_validator : public reference_counted {

public:
	explicit connection_validator(connection_data const &data);
	virtual ~connection_validator();
	
        typedef connection connection_type;
	typedef boost::intrusive_ptr<connection_type> connection_type_ptr;

	bool validate(connection_type_ptr conn, request_impl &req);
	void attach_logger(boost::intrusive_ptr<logger> const &log);
	void init(settings const &s);

private:
	boost::intrusive_ptr<receiver_matcher> matcher_;
};



}} // namespaces

#endif // XIVA_DETAILS_CONNECTION_VALIDATOR_HPP_INCLUDED
