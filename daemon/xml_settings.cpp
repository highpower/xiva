#include "acsetup.hpp"
#include "xml_settings.hpp"

#include <iostream>
#include <stdexcept>
#include <boost/lexical_cast.hpp>
#include <libxml/xinclude.h>

#include "xiva/error.hpp"
#include "xml_enumeration.hpp"
#include "xml_variable_map.hpp"
#include "command_line.hpp"

namespace xiva { namespace daemon {

xml_settings::xml_settings(char const *name) :
	doc_(0), vars_(new xml_variable_map())
{
	init(name);
}

xml_settings::xml_settings(int argc, char *argv[]) :
	doc_(0), vars_(new xml_variable_map())
{
	command_line args(argc, argv);
	if (args.is_help_mode()) {
		command_line::print_usage(std::cout);
		exit(0);
	}
	else if (!args.is_config_file_set()) {
		command_line::print_usage(std::cerr);
		exit(1);
	}
	init(args.config_file().c_str());
	if (args.is_dump_mode()) {
		std::string const &var = args.dump_variable();
		std::cout << var << ":" << value(var.c_str()) << std::endl;
		exit(0);
	}
}

xml_settings::~xml_settings() {
}

std::string
xml_settings::address() const {
	return value("/" XIVA_PACKAGE_NAME "/endpoint/address");
}

unsigned short
xml_settings::port() const {
	return boost::lexical_cast<unsigned short>(value("/" XIVA_PACKAGE_NAME "/endpoint/port"));
}

unsigned short
xml_settings::backlog() const {
	return boost::lexical_cast<unsigned short>(value("/" XIVA_PACKAGE_NAME "/endpoint/backlog"));
}

unsigned short
xml_settings::read_timeout() const {
	return boost::lexical_cast<unsigned short>(value("/" XIVA_PACKAGE_NAME "/read-timeout"));
}

unsigned short
xml_settings::write_timeout() const {
	return boost::lexical_cast<unsigned short>(value("/" XIVA_PACKAGE_NAME "/write-timeout"));
}

unsigned int
xml_settings::inactive_timeout() const {
	return boost::lexical_cast<unsigned int>(value("/" XIVA_PACKAGE_NAME "/inactive-timeout"));
}

unsigned short
xml_settings::listener_threads() const {
	return boost::lexical_cast<unsigned short>(value("/" XIVA_PACKAGE_NAME "/listener-threads"));
}

unsigned short
xml_settings::matcher_threads() const {
	return boost::lexical_cast<unsigned short>(value("/" XIVA_PACKAGE_NAME "/matcher-threads"));
}

std::string
xml_settings::value(std::string const &name) const {

	std::string result;
	xml::xpath_context ctx(xmlXPathNewContext(doc_.get()));
	xml::throw_unless(ctx);
	xml::xpath_object object(xmlXPathEvalExpression((xmlChar const*) name.c_str(), ctx.get()));
	xml::throw_unless(object);
	
	xmlNodeSetPtr ns = object->nodesetval;
	if (ns && ns->nodeNr) {
		char const *value = xml::text_value(ns->nodeTab[0]);
		if (value) {
			result.assign(value);
		}
	}
	else throw error("nonexistent config param: %s", name.c_str());
	vars_->resolve_variables(result);
	return result;
}

enumeration<std::string>::ptr_type
xml_settings::value_list(std::string const &name) const {
	return enumeration<std::string>::ptr_type(new
		 xml_enumeration<std::string>(doc_.get(), vars_, name));
}

void
xml_settings::find_variables() {

	xml::xpath_context ctx(xmlXPathNewContext(doc_.get()));
	xml::throw_unless(ctx);
	
	xml::xpath_object object(xmlXPathEvalExpression((xmlChar const*) "/" XIVA_PACKAGE_NAME "/variables/variable", ctx.get()));
	xml::throw_unless(object);
	
	xmlNodeSetPtr ns = object->nodesetval;
	if (!ns || 0 == ns->nodeNr) {
		return;
	}

	for (std::size_t i = 0; i < static_cast<std::size_t>(ns->nodeNr); ++i) {
		xmlNodePtr node = ns->nodeTab[i];
		xml::throw_unless(node);
		
		char const *val = xml::text_value(node);
		char const *name = xml::attr_value(node, "name");
		
		if (name && val) {
			vars_->add_variable(name, val);
		}
		else throw error("bad variable definition");
	}
}

void
xml_settings::init(char const *name) {
	doc_.reset(xmlParseFile(name));
	xml::throw_unless(doc_);
	if (0 == xmlDocGetRootElement(doc_.get())) {
		throw error("empty config %s", name);
	}
	xml::throw_unless(xmlXIncludeProcess(doc_.get()) >= 0);
	find_variables();
}

}} // namespaces
