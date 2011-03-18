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

#ifndef XIVA_DETAILS_FORMATTERS_FACTORY_HPP_INCLUDED
#define XIVA_DETAILS_FORMATTERS_FACTORY_HPP_INCLUDED

#include <string>
#include <map>
#include <memory>

#include <boost/intrusive_ptr.hpp>

#include "xiva/forward.hpp"
#include "xiva/formatter.hpp"
#include "details/formatters_data.hpp"

namespace xiva { namespace details {

class formatters_factory {

public:
	formatters_factory();
	virtual ~formatters_factory();

	std::auto_ptr<formatter> find(std::string const &fmt_id, request const &req) const;

	void attach_creator(std::string const &fmt_id, boost::intrusive_ptr<formatter_creator> const &fmt_ptr);

	std::auto_ptr<formatters_data> create_formatters_data(request_impl const &req, response_impl const &resp) const;

private:
	formatters_factory(formatters_factory const &);
	formatters_factory& operator = (formatters_factory const &);

	typedef std::map< std::string, boost::intrusive_ptr<formatter_creator> > formatters_type;

	formatters_type formatters_;
};


}} // namespaces

#endif // XIVA_DETAILS_FORMATTERS_FACTORY_HPP_INCLUDED
