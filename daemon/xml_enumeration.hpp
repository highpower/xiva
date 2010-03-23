/** @file xml_enumeration.hpp */
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

#ifndef XIVA_DAEMON_XML_ENUMERATION_HPP_INCLUDED
#define XIVA_DAEMON_XML_ENUMERATION_HPP_INCLUDED

#include <string>
#include <cassert>
#include <boost/lexical_cast.hpp>
#include <boost/intrusive_ptr.hpp>

#include "xml.hpp"
#include "variable_map.hpp"
#include "xiva/enumeration.hpp"

namespace xiva { namespace daemon {

template <typename Item>
class xml_enumeration : public enumeration<Item> {

public:
	xml_enumeration(xmlDocPtr doc, boost::intrusive_ptr<variable_map> const &vars, char const *name);
	virtual ~xml_enumeration();
	
	virtual Item next() const;
	virtual bool empty() const;

private:
	xml_enumeration(xml_enumeration const &);
	xml_enumeration& operator = (xml_enumeration const &);

private:
	xmlNodeSetPtr ns_;
	xml::xpath_object obj_;
	xml::xpath_context ctx_;
	boost::intrusive_ptr<variable_map> vars_;
	mutable std::size_t count_;
};

template <typename Item> inline
xml_enumeration<Item>::xml_enumeration(xmlDocPtr doc, boost::intrusive_ptr<variable_map> const &vars, char const *name) : 
	ns_(0), vars_(vars), count_(0)
{
	ctx_.reset(xmlXPathNewContext(doc));
	xml::throw_unless(ctx_);
	
	obj_.reset(xmlXPathEvalExpression((xmlChar const*) name, ctx_.get()));
	xml::throw_unless(obj_);
	
	ns_ = obj_->nodesetval;
}

template <typename Item> inline
xml_enumeration<Item>::~xml_enumeration() {
}

template <typename Item> inline Item
xml_enumeration<Item>::next() const {
	
	assert(ns_);
	xmlNodePtr node = ns_->nodeTab[count_];
	++count_;
	
	std::string buffer(xml::text_value(node));
	vars_->resolve_variables(buffer);
	return boost::lexical_cast<Item>(buffer);
}

template <typename Item> inline bool
xml_enumeration<Item>::empty() const {
	return !ns_ || count_ == static_cast<std::size_t>(ns_->nodeNr);
}

}} // namespaces

#endif // XIVA_DAEMON_XML_ENUMERATION_HPP_INCLUDED
