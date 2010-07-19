#include "acsetup.hpp"
#include "details/formatters_data_simple.hpp"

#include "xiva/message.hpp"
#include "xiva/request.hpp"
#include "xiva/formatter.hpp"

#include "details/response_impl.hpp"
#include "details/formatters_factory.hpp"

namespace xiva { namespace details {

formatters_data_simple::formatters_data_simple(
	formatters_factory const &factory, request_impl const &req, response_impl const &resp)
{
	assert(resp.channels_data().empty());

	std::string const &fmt_id = resp.default_formatter_id();
	if (!fmt_id.empty()) {
		request request_adapter(req);
		fmt_ptr_ = factory.find(fmt_id, request_adapter);
	}
}

formatters_data_simple::~formatters_data_simple() {
}

bool
formatters_data_simple::allow_message(message const& msg) const {
	(void) msg;
	return true;
}

formatter const*
formatters_data_simple::default_formatter() const {
	return fmt_ptr_.get();
}

formatter const*
formatters_data_simple::find_formatter(message const &msg) const {
	(void) msg;
	return fmt_ptr_.get();
}

} } // namespaces
