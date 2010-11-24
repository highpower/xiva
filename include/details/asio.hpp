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

#ifndef XIVA_DETAILS_ASIO_HPP_INCLUDED
#define XIVA_DETAILS_ASIO_HPP_INCLUDED

#if XIVA_HAVE_BOOST_ASIO

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

#include <boost/asio/ssl.hpp>

namespace asio = boost::asio;
namespace syst = boost::system;

#define XIVA_BEGIN_ASIO_NAMESPACE namespace boost { namespace asio {
#define XIVA_END_ASIO_NAMESPACE }}

#else // XIVA_HAVE_BOOST_ASIO

#include <asio.hpp>
#include <asio/ssl.hpp>

namespace syst = asio;

#define XIVA_BEGIN_ASIO_NAMESPACE namespace asio {
#define XIVA_END_ASIO_NAMESPACE }

#endif // XIVA_HAVE_BOOST_ASIO

#endif // XIVA_DETAILS_ASIO_HPP_INCLUDED
