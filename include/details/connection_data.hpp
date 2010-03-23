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
#include "details/http_constants.hpp"
#include "details/functors.hpp"

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

	boost::intrusive_ptr<receiver_matcher> const& matcher() const;
	void matcher(boost::intrusive_ptr<receiver_matcher> const &m);

	//boost::intrusive_ptr<connection_manager> const& manager() const;
	//void manager(boost::intrusive_ptr<connection_manager> const &m);

	template <typename Iter>
	static bool is_policy(Iter begin, Iter end);

	std::string const& policy_data() const;

private:
	unsigned short read_timeout_, write_timeout_;
	unsigned int inactive_timeout_;
	boost::intrusive_ptr<receiver_matcher> matcher_;
	//boost::intrusive_ptr<connection_manager> manager_;
	std::string policy_data_;
};

template <typename Iter> bool
connection_data::is_policy(Iter begin, Iter end) {
	if (std::distance(begin, end) < http_constants::policy_file_request.size()) {
		return false;
	}
	return std::equal(
		http_constants::policy_file_request.begin(), http_constants::policy_file_request.end(), begin, ci_equal<char>());
}

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

inline std::string const&
connection_data::policy_data() const {
	return policy_data_;
}


}} // namespaces

#endif // XIVA_DETAILS_CONNECTION_DATA_HPP_INCLUDED
