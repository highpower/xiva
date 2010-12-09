#include "acsetup.hpp"
#include "xiva/server.hpp"
#include "details/server_impl.hpp"

namespace xiva {

server::server() :
	impl_(new details::server_impl())
{
}

server::~server() {
}

void
server::stop() {
	impl_->stop();
}

void
server::start(settings const &s) {
	impl_->init(s);
	impl_->start();
}

void
server::send(std::string const &to, boost::shared_ptr<message> const &m) {
	impl_->send(to, m);
}

void
server::send(globals::connection_id to, boost::shared_ptr<message> const &m) {
	impl_->send(to, m);
}

void
server::notify_connection_opened_failed(std::string const &to, globals::connection_id id) {
	impl_->notify_connection_opened_failed(to, id, true);
}

component_set&
server::components() {
	return *impl_;
}

void
server::start_provider(unsigned short nthreads, boost::function<globals::provider_type> f) {
	impl_->start_provider(nthreads, f);
}

void
server::attach_logger(boost::intrusive_ptr<logger> const &log) {
	impl_->attach_logger(log);
}

void
server::attach_response_handler(boost::intrusive_ptr<response_handler> const &m) {
	impl_->attach_response_handler(m);
}

void
server::attach_formatter_creator(std::string const &fmt_id, boost::intrusive_ptr<formatter_creator> const &creator) {
	impl_->attach_formatter_creator(fmt_id, creator);
}

void
server::add_connection_listener(boost::intrusive_ptr<connection_listener> const &l) {
	impl_->add_connection_listener(l);
}

void
server::attach_message_filter(boost::intrusive_ptr<message_filter> const &filter) {
	impl_->attach_message_filter(filter);
}

} // namespace
