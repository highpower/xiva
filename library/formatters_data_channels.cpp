#include "acsetup.hpp"
#include "details/formatters_data_channels.hpp"

#include <cassert>

#include "xiva/message.hpp"
#include "xiva/message_filter.hpp"
#include "xiva/request.hpp"
#include "xiva/formatter.hpp"
#include "xiva/channel_info.hpp"

#include "details/channels_stat_impl.hpp"
#include "details/response_impl.hpp"
#include "details/formatters_factory.hpp"

namespace xiva { namespace details {

formatters_data_channels::formatters_data_channels(
	formatters_factory const &factory, request_impl const &req, response_impl const &resp) : default_formatter_(NULL)
{
	std::map<channel_info, std::string> ch_data = resp.channels_data();
	assert(!ch_data.empty());

	request request_adapter(req);

	for (std::map<channel_info, std::string>::iterator it = ch_data.begin(), end = ch_data.end(); it != end; ++it) {
		channel_info const &ch_info = it->first;
		assert(!ch_info.empty());

		std::string const &fmt_id = it->second;
		if (fmt_id.empty()) {
			channels_data_[ch_info] = channel_data(ch_info.data(), NULL);
			continue;
		}
		std::map<std::string, formatter*>::const_iterator fmt_it = formatters_.find(fmt_id);
		if (formatters_.end() != fmt_it) {
			channels_data_[ch_info] = channel_data(ch_info.data(), fmt_it->second);
		}
		else {
			std::auto_ptr<formatter> fmt_ptr = factory.find(fmt_id, request_adapter);
			channels_data_[ch_info] = channel_data(ch_info.data(), fmt_ptr.get());
			formatters_.insert(std::make_pair(fmt_id, fmt_ptr.get()));
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
formatters_data_channels::allow_message(message const& msg, message_filter const *filter) const {

	channel_info const &msg_ch_info = msg.get_channel_info();
	if (msg_ch_info.empty()) {
		return false;
	}

	channels_data::const_iterator ch_it = channels_data_.find(msg_ch_info);
	if (channels_data_.end() == ch_it) {
		return false;
	}
	channel_data const &ch_data = ch_it->second;
	return NULL == filter || filter->allow_message(msg, ch_data.first, ch_data.second);
}

formatter const*
formatters_data_channels::default_formatter() const {
	return default_formatter_;
}

formatter const*
formatters_data_channels::find_formatter(message const &msg) const {

	channel_info const &msg_ch_info = msg.get_channel_info();
	assert(!msg_ch_info.empty());

	channels_data::const_iterator ch_it = channels_data_.find(msg_ch_info);
	if (channels_data_.end() == ch_it) {
		assert(false);
		return NULL; // unreachable code
	}
	channel_data const &ch_data = ch_it->second;
	return ch_data.second;
}

void
formatters_data_channels::update(message const& msg) {

	channel_info const &msg_ch_info = msg.get_channel_info();
	assert(!msg_ch_info.empty());

	if (!msg_ch_info.data().empty()) {
		channels_data::iterator ch_it = channels_data_.find(msg_ch_info);
		if (channels_data_.end() != ch_it) {
			channel_data &ch_data = ch_it->second;
			ch_data.first.assign(msg_ch_info.data());
		}
	}
}

void
formatters_data_channels::update_channels_stat(channels_stat_impl &ch_stat, bool add) const {

	for (channels_data::const_iterator it = channels_data_.begin(), end = channels_data_.end(); it != end; ++it) {
		if (add) {
			ch_stat.inc(it->first);
		}
		else {
			ch_stat.dec(it->first);
		}
	}
}

} } // namespaces
