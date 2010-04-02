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

#ifndef XIVA_TESTS_MOCK_LISTENER_HPP_INCLUDED
#define XIVA_TESTS_MOCK_LISTENER_HPP_INCLUDED

#include <map>
#include <set>
#include <string>
#include <cstddef>

#include "xiva/globals.hpp"
#include "xiva/connection_listener.hpp"

namespace xiva { namespace tests {

class mock_listener : public connection_listener {

public:
	mock_listener();
	virtual ~mock_listener();

	bool available(std::string const &user) const;
	virtual void connection_opened(std::string const &to, globals::connection_id id) throw (std::exception);
	virtual void connection_closed(std::string const &to, globals::connection_id id) throw (std::exception);

private:
	mock_listener(mock_listener const &);
	mock_listener& operator = (mock_listener const &);

private:
	typedef std::set<globals::connection_id> connection_ids;
	std::map<std::string, connection_ids> available_;
};

}} // namespaces

#endif // XIVA_TESTS_MOCK_LISTENER_HPP_INCLUDED
