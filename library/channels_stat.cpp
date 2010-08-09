#include "acsetup.hpp"
#include "xiva/channels_stat.hpp"

#include <cassert>

#include "details/channels_stat_impl.hpp"


namespace xiva {

channels_stat::channels_stat(boost::shared_ptr<details::channels_stat_impl> const &impl)
	: impl_(impl)
{
	assert(impl);
}

channels_stat::~channels_stat() {
}

enumeration<std::string>::ptr_type
channels_stat::load_names() const {
	return impl_->load_names();
}

enumeration<std::string>::ptr_type
channels_stat::load_keys(std::string const &ch_name) const {
	return impl_->load_keys(ch_name);
}

} // namespace
