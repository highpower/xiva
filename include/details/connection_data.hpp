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

#include <iterator>
#include <map>
#include <memory>
#include <boost/intrusive_ptr.hpp>

#include "xiva/forward.hpp"
#include "xiva/logger.hpp"

#include "details/http_constants.hpp"
#include "details/functors.hpp"

namespace xiva { namespace details {

class connection_data {

public:
	connection_data();
	virtual ~connection_data();

	void init(settings const &s);
	unsigned int read_timeout() const;
	unsigned int write_timeout() const;
	unsigned int inactive_timeout() const;
	unsigned int ping_interval() const;

	boost::intrusive_ptr<response_handler> const& handler() const;
	void handler(boost::intrusive_ptr<response_handler> const &h);

	boost::intrusive_ptr<logger> const& log() const;
	void attach_logger(boost::intrusive_ptr<logger> const &log);

	template <typename Iter>
	static bool is_policy(Iter begin, Iter end);

	std::string const& policy_data() const;

	std::auto_ptr<formatter> find_formatter(std::string const &fmt_id, request const &req) const;
	void attach_formatter_creator(std::string const &fmt_id, boost::intrusive_ptr<formatter_creator> const &fmt_ptr);

	bool stopping() const;
	void stop();

private:
	connection_data(connection_data const &);
	connection_data & operator = (connection_data const &);

	typedef std::map< std::string, boost::intrusive_ptr<formatter_creator> > formatters_type;

	formatters_type formatters_;
	std::string policy_data_;
	boost::intrusive_ptr<logger> logger_;
	boost::intrusive_ptr<response_handler> handler_;
	unsigned int read_timeout_, write_timeout_, inactive_timeout_;
	unsigned int ping_interval_;
	bool stopping_;
};

template <typename Iter> bool
connection_data::is_policy(Iter begin, Iter end) {

	typedef typename std::iterator_traits<Iter>::value_type char_type;
	if (std::distance(begin, end) < http_constants<char_type>::policy_file_request.size()) {
		return false;
	}
	return std::equal(http_constants<char_type>::policy_file_request.begin(), 
		http_constants<char_type>::policy_file_request.end(), begin, ci_equal<char>());
}

inline unsigned int
connection_data::read_timeout() const {
	return read_timeout_;
}

inline unsigned int
connection_data::write_timeout() const {
	return write_timeout_;
}

inline unsigned int
connection_data::inactive_timeout() const {
	return inactive_timeout_;
}

inline unsigned int
connection_data::ping_interval() const {
	return ping_interval_;
}

inline std::string const&
connection_data::policy_data() const {
	return policy_data_;
}

inline boost::intrusive_ptr<logger> const&
connection_data::log() const {
	return logger_;
}

}} // namespaces

#endif // XIVA_DETAILS_CONNECTION_DATA_HPP_INCLUDED
