#include "acsetup.hpp"
#include "xiva/response.hpp"
#include "details/response_impl.hpp"

namespace xiva {

response::response(details::response_impl &impl) :
	impl_(impl)
{
}

response::~response() {
}

} // namespaces
