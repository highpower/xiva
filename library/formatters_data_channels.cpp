#include "acsetup.hpp"
#include "details/formatters_data_channels.hpp"

#include <cassert>

#include "xiva/message.hpp"
#include "xiva/request.hpp"
#include "xiva/formatter.hpp"

#include "details/response_impl.hpp"
#include "details/formatters_factory.hpp"

namespace xiva { namespace details {

formatters_data_channels::formatters_data_channels(
	formatters_factory const &factory, request_impl const &req, response_impl const &resp) : default_formatter_(NULL)
{
	std::map<std::string, std::string> ch_data = resp.channels_data();
	assert(!ch_data.empty());

	request request_adapter(req);

	for (std::map<std::string, std::string>::iterator it = ch_data.begin(), end = ch_data.end(); it != end; ++it) {
		std::string const &channel = it->first;
		std::string const &fmt_id = it->second;
		if (fmt_id.empty()) {
			channels_data_[channel] = NULL;
			continue;
		}
		std::map<std::string, formatter*>::const_iterator fmt_it = formatters_.find(fmt_id);
		if (formatters_.end() != fmt_it) {
			channels_data_.insert(make_pair(channel, fmt_it->second));
		}
		else {
			std::auto_ptr<formatter> fmt_ptr = factory.find(fmt_id, request_adapter);
			channels_data_.insert(make_pair(channel, fmt_ptr.get()));
			formatters_.insert(make_pair(fmt_id, fmt_ptr.get()));
			fmt_ptr.release();
		}
	}

	std::string const &def_fmt_id = resp.default_formatter_id();
	if (!def_fmt_id.empty()) {
		std::map<std::string, formatter*>::const_iterator it = formatters_.find(def_fmt_id);
		if (formatters_.end() != it) {
			default_formatter_ = it->second;
		}
		else {
			std::auto_ptr<formatter> fmt_ptr = factory.find(def_fmt_id, request_adapter);
			formatters_.insert(make_pair(def_fmt_id, fmt_ptr.get()));
			default_formatter_ = fmt_ptr.release();
		}
	}
}

formatters_data_channels::~formatters_data_channels() {
	for (std::map<std::string, formatter*>::iterator it = formatters_.begin(), end = formatters_.end(); it != end; ++it) {
		formatter *fmt_ptr = it->second;
		if (NULL != fmt_ptr) {
			delete fmt_ptr;
		}
	}
}

bool
formatters_data_channels::allow_message(message const& msg) const {
	std::set<std::string> const &channels = msg.channels();
	if (channels.empty()) {
		return false;
	}

	for (std::set<std::string>::const_iterator it = channels.begin(), end = channels.end(); it != end; ++it) {
		if (channels_data_.end() != channels_data_.find(*it)) {
			return true;
		}
	}
	return false;
}

formatter const*
formatters_data_channels::default_formatter() const {
	return default_formatter_;
}

formatter const*
formatters_data_channels::find_formatter(message const &msg) const {

	std::set<std::string> const &channels = msg.channels();
	assert(!channels.empty());

	for (std::set<std::string>::const_iterator it = channels.begin(), end = channels.end(); it != end; ++it) {
		std::map<std::string, formatter const*>::const_iterator ch_it = channels_data_.find(*it);
		if (channels_data_.end() != ch_it) {
			return ch_it->second;
		}
	}
	assert(false);
	return NULL; // not reachable code
}

} } // namespaces
