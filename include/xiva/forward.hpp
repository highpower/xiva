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

#ifndef XIVA_DETAILS_FORWARD_HPP_INCLUDED
#define XIVA_DETAILS_FORWARD_HPP_INCLUDED

namespace xiva {

class logger;
class message;
class request;
class response;
class settings;
class formatter;

class http_error;
class component_set;
class message_sender;
class response_handler;
class connection_listener;

namespace details {

class server_impl;
class request_impl;
class response_impl;

}} // namespaces

#endif // XIVA_DETAILS_FORWARD_HPP_INCLUDED
