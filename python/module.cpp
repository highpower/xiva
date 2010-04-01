#include "acsetup.hpp"

#include <boost/python.hpp>

#include "server_class.hpp"
#include "request_class.hpp"
#include "response_class.hpp"

namespace py = boost::python;

namespace xiva { namespace python {

BOOST_PYTHON_MODULE(xiva) {
	register_server_class();
	register_request_class();
	register_response_class();
}

}} // namespaces
