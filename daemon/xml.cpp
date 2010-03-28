#include "acsetup.hpp"
#include "xml.hpp"

#include <stdexcept>
#include <libxml/xmlerror.h>

#include "xiva/error.hpp"

namespace xiva { namespace daemon { namespace xml {

struct xml_setup {
	xml_setup();
};

void
throw_unless(bool condition) {
	if (condition) {
		return;
	}
	char const *message = "unknown XML error";
	xmlErrorPtr xml_err = xmlGetLastError();
	if (xml_err && xml_err->message) {
		message = xml_err->message;
	}
	error err("%s", message);
	xmlResetLastError();
	throw err;
}

void
doc_traits::destroy(xmlDocPtr doc) {
	xmlFreeDoc(doc);
}

void
xpath_object_traits::destroy(xmlXPathObjectPtr object) {
	xmlXPathFreeObject(object);
}

void
xpath_context_traits::destroy(xmlXPathContextPtr context) {
	xmlXPathFreeContext(context);
}

char const*
attr_value(xmlNodePtr node, char const *name) {
	assert(node);
	xmlAttrPtr attr = xmlHasProp(node, (xmlChar const*) name);
	return attr ? text_value(attr) : 0;
}

extern "C" void
	xiva_xml_null_error(void *ctx, char const *format, ...) {

	(void) ctx;
	(void) format;

}

xml_setup::xml_setup() {
	xmlSetGenericErrorFunc(0, &xiva_xml_null_error);
}

static xml_setup setup_;

}}} // namespaces
