#include "acsetup.hpp"
#include "details/server_impl.hpp"

#include <cassert>
#include <boost/ref.hpp>
#include <boost/bind.hpp>

#include "xiva/logger.hpp"
#include "xiva/settings.hpp"

#include "details/acceptor.hpp"
#include "details/url_matcher.hpp"
#include "details/stdio_logger.hpp"
#include "details/message_queue.hpp"
#include "details/threaded_listener.hpp"
#include "details/connection.hpp"
#include "details/matcher_invoker.hpp"
#include "details/threaded_connection.hpp"
#include "details/threaded_matcher_invoker.hpp"
#include "details/connection_manager.hpp"
#include "details/connection_traits.hpp"

namespace xiva { namespace details {

server_impl::server_impl() :
	io_(), strand_(io_), data_()
{
	listener_ = boost::intrusive_ptr<compound_listener>(new threaded_listener());
}

server_impl::~server_impl() {
}

void
server_impl::stop() {
	if (acceptor_) {
		acceptor_->stop();
	}
	if (message_queue_) {
		message_queue_->finish();
	}
	strand_.dispatch(boost::bind(&server_impl::stop_service, this));
	join_all();
	if (connection_manager_) {
		connection_manager_->wait_for_complete();
	}
	io_.stop();
}

void
server_impl::start(settings const &s) {

	if (!logger_) {
		attach_logger(boost::intrusive_ptr<logger>(new stdio_logger()));
	}
	data_.attach_logger(logger_);
	data_.init(s);

	boost::intrusive_ptr<receiver_matcher> matcher = data_.matcher();
	if (!matcher) {
		attach_receiver_matcher(boost::intrusive_ptr<receiver_matcher>(new url_matcher()));
	}

	if (data_.matcher()->threaded()) {
		typedef threaded_matcher_invoker invoker_type;
		typedef invoker_type::connection_type connection_type;
		typedef connection_manager<connection_type> manager_type;
		typedef connection_traits<connection_type, invoker_type> traits_type;
		typedef acceptor<connection_type, traits_type> acceptor_type;

		boost::intrusive_ptr<manager_type> cm(new manager_type(listener_));
		boost::intrusive_ptr<invoker_type> cv(new invoker_type(io_, data_));
		boost::intrusive_ptr<traits_type> ct(new traits_type(cm, cv));
		acceptor_ = boost::intrusive_ptr<acceptor_base>(new acceptor_type(io_, data_, *ct));
		connection_manager_ = cm;
		connection_traits_ = ct;
	}
	else {
		typedef matcher_invoker invoker_type;
		typedef invoker_type::connection_type connection_type;
		typedef connection_manager<connection_type> manager_type;
		typedef connection_traits<connection_type, invoker_type> traits_type;
		typedef acceptor<connection_type, traits_type> acceptor_type;

		boost::intrusive_ptr<manager_type> cm(new manager_type(listener_));
		boost::intrusive_ptr<invoker_type> cv(new invoker_type(data_));
		boost::intrusive_ptr<traits_type> ct(new traits_type(cm, cv));
		acceptor_ = boost::intrusive_ptr<acceptor_base>(new acceptor_type(io_, data_, *ct));
		connection_manager_ = cm;
		connection_traits_ = ct;
	}

	message_queue_ = boost::intrusive_ptr<message_queue>(new message_queue(io_, connection_manager_));
	acceptor_->attach_logger(logger_);
	message_queue_->attach_logger(logger_);
	connection_traits_->attach_logger(logger_);
	connection_traits_->init(s);

	acceptor_->bind(s.address(), s.port(), s.backlog());
	acceptor_->accept();
	for (std::vector<thread_param_type>::const_iterator i = providers_.begin(), end = providers_.end(); i != end; ++i) {
		start_provider_thread(*i);
	}
	io_.run();
}

void
server_impl::send(std::string const &to, boost::shared_ptr<message> const &m) {
	if (message_queue_) {
		message_queue_->send(to, m);
	}
}

void
server_impl::send(globals::connection_id const &to, boost::shared_ptr<message> const &m) {
	if (message_queue_) {
		message_queue_->send(to, m);
	}
}

void
server_impl::attach_logger(boost::intrusive_ptr<logger> const &log) {

	assert(log);
	logger_ = log;
}

void
server_impl::attach_receiver_matcher(boost::intrusive_ptr<receiver_matcher> const &m) {
	assert(m);
	matcher_ = m;
	data_.matcher(m);
}

void
server_impl::add_connection_listener(boost::intrusive_ptr<connection_listener> const &l) {
	assert(l);
	listener_->add_connection_listener(l);
}

void
server_impl::start_provider(unsigned short nthreads, boost::function<globals::provider_type> f) {
	providers_.push_back(thread_param_type(f, nthreads));
}

void
server_impl::stop_service() {
	if (acceptor_) {
		acceptor_->stop();
	}
	if (connection_manager_) {
		connection_manager_->finish();
	}
}

void
server_impl::start_provider_thread(server_impl::thread_param_type const &tp) {
	boost::function<void()> thread_func = boost::bind(&server_impl::provider_thread_func, this, tp.first);
	for (unsigned short i = 0; i < tp.second; ++i) {
		create_thread(thread_func);
	}
}

void
server_impl::provider_thread_func(boost::function<globals::provider_type> f) {
	try {
		f(*message_queue_);
	}
	catch (std::exception const &e) {
		logger_->error("caught exception in provider function: %s", e.what());
	}
}

}} // namespaces
