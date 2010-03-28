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

#ifndef XIVA_DETAILS_SYSLOG_LOGGER_HPP_INCLUDED
#define XIVA_DETAILS_SYSLOG_LOGGER_HPP_INCLUDED

#include <cstdarg>

#include "xiva/logger.hpp"

namespace xiva { namespace details {

class stdio_logger : public logger {

public:
	stdio_logger();
	virtual ~stdio_logger();

	virtual void info(char const *format, ...);
	virtual void debug(char const *format, ...);
	virtual void error(char const *format, ...);

private:
	stdio_logger(stdio_logger const &);
	stdio_logger& operator = (stdio_logger const &);
	void print_message(char const *level, char const *format, va_list args);
};

}} // namespaces

#endif // XIVA_DETAILS_SYSLOG_LOGGER_HPP_INCLUDED
