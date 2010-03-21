/** @file xml.hpp */
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

#ifndef XIVA_DAEMON_XML_HPP_INCLUDED
#define XIVA_DAEMON_XML_HPP_INCLUDED

#include <cassert>
#include <libxml/tree.h>
#include <libxml/xpath.h>

#include "details/resource.hpp"

namespace xiva { namespace daemon { namespace xml {

void
throw_unless(bool condition);

struct doc_traits : public details::default_traits<xmlDocPtr> {
	void destroy(xmlDocPtr doc);
};

typedef details::resource<xmlDocPtr, doc_traits> doc;

struct xpath_object_traits : public details::default_traits<xmlXPathObjectPtr> {
	void destroy(xmlXPathObjectPtr object);
};

typedef details::resource<xmlXPathObjectPtr, xpath_object_traits> xpath_object;

struct xpath_context_traits : public details::default_traits<xmlXPathContextPtr> {
	void destroy(xmlXPathContextPtr context);
};

typedef details::resource<xmlXPathContextPtr, xpath_context_traits> xpath_context;

template <typename Node> char const*
text_value(Node node) {
	assert(node);
	xmlNodePtr child = node->children;
	return (child && xmlNodeIsText(child) && child->content) ? (char const*) child->content : 0;
}

char const*
attr_value(xmlNodePtr node, char const *name);

}}} // namespaces

#endif // XIVA_DAEMON_XML_HPP_INCLUDED
