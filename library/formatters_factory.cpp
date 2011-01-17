#include "acsetup.hpp"
#include "details/formatters_factory.hpp"

#include "xiva/formatter.hpp"
#include "xiva/formatter_creator.hpp"
#include "xiva/response_handler.hpp"

#include "details/response_impl.hpp"
#include "details/formatters_data_channels.hpp"
#include "details/formatters_data_simple.hpp"


namespace xiva { namespace details {

formatters_factory::formatters_factory()
{
}

formatters_factory::~formatters_factory() {
}

std::auto_ptr<formatter>
formatters_factory::find(std::string const &fmt_id, request const &req) const {
	formatters_type::const_iterator i = formatters_.find(fmt_id);
	if (formatters_.end() != i) {
		formatter_creator const *creator = i->second.get();
		if (NULL != creator) {
			return creator->create(req);
		}
	}
	return std::auto_ptr<formatter>();
}

void
formatters_factory::attach_creator(std::string const &fmt_id, boost::intrusive_ptr<formatter_creator> const &creator) {
	formatters_.insert(std::make_pair(fmt_id, creator));
}

std::auto_ptr<formatters_data>
formatters_factory::create_formatters_data(request_impl const &req, response_impl const &resp) const {
	if (!resp.channels_data().empty()) {
		return std::auto_ptr<formatters_data>(new formatters_data_channels(*this, req, resp));
	}
	return formatters_data_simple::create(*this, req, resp);
}

}} // namespaces
