/** @file message_sender.hpp */
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

#ifndef XIVA_MESSAGE_SENDER_HPP_INCLUDED
#define XIVA_MESSAGE_SENDER_HPP_INCLUDED

#include <string>
#include <boost/shared_ptr.hpp>

#include "xiva/config.hpp"
#include "xiva/globals.hpp"

namespace xiva {

class message;

class XIVA_API message_sender {

public:
	message_sender();
	virtual ~message_sender();
	
	virtual bool active() const = 0;
	virtual void send(std::string const &to, boost::shared_ptr<message> const &m) = 0;
	virtual void send(globals::connection_id const &to, boost::shared_ptr<message> const &m) = 0;

private:
	message_sender(message_sender const &);
	message_sender& operator = (message_sender const &);
};

} // namespace

#endif // XIVA_MESSAGE_SENDER_HPP_INCLUDED
