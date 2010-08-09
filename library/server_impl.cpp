#include "acsetup.hpp"
#include "details/server_impl.hpp"

#include <cassert>
#include <boost/ref.hpp>
#include <boost/bind.hpp>

#include "xiva/logger.hpp"
#include "xiva/settings.hpp"
#include "xiva/message_filter.hpp"

#include "details/connection_data.hpp"
#include "details/acceptor.hpp"
#include "details/connection.hpp"
#include "details/threaded_connection.hpp"

#include "details/stdio_logger.hpp"
#include "details/message_queue.hpp"
#include "details/threaded_listener.hpp"
#include "details/formatters_factory.hpp"

#include "details/handler_invoker.hpp"
#include "details/threaded_handler_invoker.hpp"

#include "details/connection_traits.hpp"
#include "details/connection_manager.hpp"
#include "details/url_response_handler.hpp"

namespace xiva { namespace details {

server_impl::server_impl() :
	io_(), strand_(io_), started_(false)
{
	listener_ = boost::intrusive_ptr<threaded_listener>(new threaded_listener());
	data_.reset(new connection_data());
}

server_impl::~server_impl() {
}

void
server_impl::stop() {
	data_->stop();
	if (acceptor_) {
		acceptor_->stop();
	}
	listener_->finish();
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
server_impl::init(settings const &s) {

	if (!logger_) {
		attach_logger(boost::intrusive_ptr<logger>(new stdio_logger()));
	}
	data_->attach_logger(logger_);
	data_->init(s);

	if (!data_->handler()) {
		attach_response_handler(boost::intrusive_ptr<response_handler>(new url_response_handler()));
	}

	if (data_->handler()->threaded()) {
		typedef threaded_handler_invoker invoker_type;
		typedef invoker_type::connection_type connection_type;
		typedef connection_manager<connection_type> manager_type;
		typedef connection_traits<connection_type, invoker_type> traits_type;
		typedef acceptor<connection_type, traits_type> acceptor_type;

		boost::intrusive_ptr<manager_type> cm(new manager_type(listener_));
		boost::intrusive_ptr<invoker_type> cv(new invoker_type(io_, *data_));
		boost::intrusive_ptr<traits_type> ct(new traits_type(cm, cv));
		acceptor_ = boost::intrusive_ptr<acceptor_base>(new acceptor_type(io_, *data_, *ct));
		connection_manager_ = cm;
		connection_traits_ = ct;
	}
	else {
		typedef handler_invoker invoker_type;
		typedef invoker_type::connection_type connection_type;
		typedef connection_manager<connection_type> manager_type;
		typedef connection_traits<connection_type, invoker_type> traits_type;
		typedef acceptor<connection_type, traits_type> acceptor_type;

		boost::intrusive_ptr<manager_type> cm(new manager_type(listener_));
		boost::intrusive_ptr<invoker_type> cv(new invoker_type(*data_));
		boost::intrusive_ptr<traits_type> ct(new traits_type(cm, cv));
		acceptor_ = boost::intrusive_ptr<acceptor_base>(new acceptor_type(io_, *data_, *ct));
		connection_manager_ = cm;
		connection_traits_ = ct;
	}

	connection_manager_->attach_message_filter(message_filter_);
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
}

boost::shared_ptr<channels_stat> const&
server_impl::init_channels_stat() {
	assert(!started_);
	assert(connection_manager_);
	return connection_manager_->init_channels_stat();;
}

void
server_impl::start() {
	started_ = true;
	try {
		io_.run();
	}
	catch (std::exception const &e) {
		if (logger_ && !data_->stopping()) {
			logger_->error("server_impl::start failed, exception: %s", e.what());
		}
	}
	catch (...) {
		if (logger_ && !data_->stopping()) {
			logger_->error("server_impl::start failed, unknown exception");
		}
	}
}

void
server_impl::send(std::string const &to, boost::shared_ptr<message> const &m) {
	if (message_queue_) {
		message_queue_->send(to, m);
	}
}

void
server_impl::send(globals::connection_id to, boost::shared_ptr<message> const &m) {
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
server_impl::attach_message_filter(boost::intrusive_ptr<message_filter> const &filter) {
	assert(filter);
	message_filter_ = filter;
}

void
server_impl::attach_response_handler(boost::intrusive_ptr<response_handler> const &h) {
	assert(h);
	data_->handler(h);
}

void
server_impl::attach_formatter_creator(std::string const &fmt_id, boost::intrusive_ptr<formatter_creator> const &creator) {
	assert(creator);
	data_->fmt_factory().attach_creator(fmt_id, creator);
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
	if (connection_traits_) {
		connection_traits_->finish();
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
