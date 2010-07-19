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

#ifndef XIVA_DETAILS_FORMATTERS_DATA_SIMPLE_HPP_INCLUDED
#define XIVA_DETAILS_FORMATTERS_DATA_SIMPLE_HPP_INCLUDED

#include <memory>

#include "xiva/forward.hpp"
#include "details/formatters_data.hpp"

namespace xiva { namespace details {

class formatters_data_simple : public formatters_data {

public:
	formatters_data_simple(formatters_factory const &factory, request_impl const &req, response_impl const &resp);
	virtual ~formatters_data_simple();

	virtual bool allow_message(message const& msg) const;

	virtual formatter const* default_formatter() const;
	virtual formatter const* find_formatter(message const& msg) const;

private:
	std::auto_ptr<formatter> fmt_ptr_;
};


}} // namespaces

#endif // XIVA_DETAILS_FORMATTERS_DATA_SIMPLE_HPP_INCLUDED
