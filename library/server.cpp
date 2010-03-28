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
	impl_->start(s);
}

void
server::send(std::string const &to, boost::shared_ptr<message> const &m) {
	impl_->send(to, m);
}

void
server::send(globals::connection_id const &to, boost::shared_ptr<message> const &m) {
	impl_->send(to, m);
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
server::attach_receiver_matcher(boost::intrusive_ptr<receiver_matcher> const &m) {
	impl_->attach_receiver_matcher(m);
}

void
server::add_connection_listener(boost::intrusive_ptr<connection_listener> const &l) {
	impl_->add_connection_listener(l);
}

} // namespace
