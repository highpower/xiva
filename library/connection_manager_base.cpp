#include "acsetup.hpp"
#include "details/connection_manager_base.hpp"

#include <cassert>

#include "xiva/message_filter.hpp"
#include "xiva/channels_stat.hpp"

#include "details/connection_base.hpp"
#include "details/compound_listener.hpp"
#include "details/channels_stat_impl.hpp"

namespace xiva { namespace details {

connection_manager_base::connection_manager_base(boost::intrusive_ptr<compound_listener> const &l) :
	listener_(l)
{
	assert(l);
}

connection_manager_base::~connection_manager_base() {
}

void
connection_manager_base::init(settings const &s) {
	listener_->init(s);
}

void
connection_manager_base::attach_logger(boost::intrusive_ptr<logger> const &log) {
	listener_->attach_logger(log);
}

void
connection_manager_base::add_connection_listener(listener_ptr_type const &l) {
	listener_->add_connection_listener(l);
}

message_filter const*
connection_manager_base::msg_filter() const {
	return message_filter_.get();
}

void
connection_manager_base::attach_message_filter(boost::intrusive_ptr<message_filter> const &filter) {
	message_filter_ = filter;
}

boost::shared_ptr<channels_stat> const&
connection_manager_base::init_channels_stat() {
	if (NULL == channels_stat_impl_.get()) {
		channels_stat_impl_ = boost::shared_ptr<channels_stat_impl>(new channels_stat_impl());
		channels_stat_ = boost::shared_ptr<channels_stat>(new channels_stat(channels_stat_impl_));
	}
	return channels_stat_;
}

void
connection_manager_base::fire_connection_opened(connection_base const &conn) {

	listener_->connection_opened(conn.name(), conn.id());
	if (NULL != channels_stat_impl_.get()) {
		conn.update_channels_stat(*channels_stat_impl_, true);
	}
}

void
connection_manager_base::fire_connection_closed(connection_base const &conn) {

	listener_->connection_closed(conn.name(), conn.id());
	if (NULL != channels_stat_impl_.get()) {
		conn.update_channels_stat(*channels_stat_impl_, false);
	}
}

void
connection_manager_base::fire_disconnected(std::string const &to) {
	listener_->disconnected(to);
}


}} // namespaces
