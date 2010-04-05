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

#ifndef XIVA_RESPONSE_HANDLER_HPP_INCLUDED
#define XIVA_RESPONSE_HANDLER_HPP_INCLUDED

#include <string>

#include "xiva/config.hpp"
#include "xiva/shared.hpp"

namespace xiva {

class request;
class response;

class XIVA_API response_handler : public shared {

public:
	response_handler();
	virtual ~response_handler();

	virtual bool threaded() const = 0;
	virtual bool has_enough_data(request const &req) const = 0;
	
	virtual std::string receiver(request const &req) const = 0;
	virtual void handle_response(request const &req, response &resp) const = 0;

private:
	response_handler(response_handler const &);
	response_handler& operator = (response_handler const &);
};

} // namespace

#endif // XIVA_RESPONSE_HANDLER_HPP_INCLUDED
