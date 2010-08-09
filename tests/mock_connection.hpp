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

#ifndef XIVA_TESTS_MOCK_CONNECTION_HPP_INCLUDED
#define XIVA_TESTS_MOCK_CONNECTION_HPP_INCLUDED

#include <string>
#include <cstddef>

#include "details/connection.hpp"

namespace xiva { namespace tests {

class mock_connection : public details::connection {

public:
	mock_connection(std::string const &to);
	virtual ~mock_connection();

	bool finished() const;
	std::size_t message_count() const;

	virtual void finish();
	virtual void send(boost::shared_ptr<message> const &m);
	virtual void handled(details::request_impl const &req, details::response_impl const &resp);

private:
	mock_connection(mock_connection const &);
	mock_connection& operator = (mock_connection const &);

private:
	bool finished_;
	std::size_t message_count_;
};

}} // namespaces

#endif // XIVA_TESTS_MOCK_CONNECTION_HPP_INCLUDED
