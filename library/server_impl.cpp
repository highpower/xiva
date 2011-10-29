#include "acsetup.hpp"
#include "details/server_impl.hpp"

#include <cassert>
#include <boost/ref.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include "xiva/logger.hpp"
#include "xiva/settings.hpp"
#include "xiva/message_filter.hpp"
#include "xiva/ping_formatter.hpp"

#include "details/connection_data.hpp"
#include "details/connection.hpp"
#include "details/threaded_connection.hpp"

#include "details/stdio_logger.hpp"
#include "details/message_queue.hpp"
#include "details/threaded_listener.hpp"
#include "details/formatters_factory.hpp"

#include "details/handler_invoker.hpp"
#include "details/threaded_handler_invoker.hpp"

#include "details/connection_traits.hpp"
#include "details/ssl_connection_traits.hpp"

#include "details/connection_manager.hpp"
#include "details/acceptor.hpp"

#include "details/url_response_handler.hpp"

namespace xiva { namespace details {

server_impl::server_impl() :
	io_(), strand_(io_), started_(false), stopped_(false)
{
	data_.reset(new connection_data(*this));
	listener_ = boost::intrusive_ptr<threaded_listener>(new threaded_listener(*data_));
}

server_impl::~server_impl() {
	stop();
	join_all();

	boost::mutex::scoped_lock lock(stop_mutex_);
	if (started_ && !stopped_) {
		stop_condition_.wait(lock);
	}
}

void
server_impl::finish() {
	data_->stop();
	if (acceptor_) {
		acceptor_->stop();
	}
	if (ssl_acceptor_) {
		ssl_acceptor_->stop();
	}
	listener_->finish();
	if (message_queue_) {
		message_queue_->finish();
	}
	if (handler_invoker_) {
		handler_invoker_->finish();
	}
}

void
server_impl::stop() {
	finish();
	if (connection_manager_ && started_ && !stopped_) {
		strand_.dispatch(boost::bind(&connection_manager_base::finish, connection_manager_));
		connection_manager_->wait_for_complete();
	}
	io_.stop();
}

template <typename invoker_type>
void server_impl::init_data(settings const &s, bool ssl) {

	typedef typename invoker_type::connection_type connection_type;
	typedef connection_manager<connection_type> manager_type;
	typedef connection_traits<invoker_type> traits_type;
	typedef acceptor<traits_type> acceptor_type;

	boost::intrusive_ptr<manager_type> cm(new manager_type(listener_));
	boost::intrusive_ptr<invoker_type> inv(new invoker_type(strand_, *data_));
	handler_invoker_ = inv;
	connection_manager_ = cm;

	boost::intrusive_ptr<traits_type> ct(new traits_type(cm, inv));
	connection_traits_ = ct;
	acceptor_ = boost::intrusive_ptr<acceptor_base>(new acceptor_type(io_, strand_, *data_, *ct));

	if (ssl) {
		typedef ssl_connection_traits<invoker_type> ssl_traits_type;
		typedef acceptor<ssl_traits_type> ssl_acceptor_type;

		boost::intrusive_ptr<ssl_traits_type> ssl_ct(new ssl_traits_type(io_, cm, inv));
		ssl_acceptor_ = boost::intrusive_ptr<acceptor_base>(new ssl_acceptor_type(io_, strand_, *data_, *ssl_ct));
		ssl_connection_traits_ = ssl_ct;
		ssl_ct->init(s);
	}
}

void
server_impl::init(settings const &s) {

	assert(!message_queue_);
	if (!logger_) {
		attach_logger(boost::intrusive_ptr<logger>(new stdio_logger()));
	}
	data_->attach_logger(logger_);
	data_->init(s, ping_formatter_);

	if (!data_->handler()) {
		attach_response_handler(boost::intrusive_ptr<response_handler>(new url_response_handler()));
	}

	unsigned short ssl_port = s.ssl_port();
	if (data_->handler()->threaded()) {
		init_data<threaded_handler_invoker>(s, 0 != ssl_port);
	}
	else {
		init_data<handler_invoker>(s, 0 != ssl_port);
	}

	handler_invoker_->attach_logger(logger_);
	handler_invoker_->init(s);

	connection_manager_->attach_message_filter(message_filter_);
	message_queue_ = boost::intrusive_ptr<message_queue>(new message_queue(strand_, connection_manager_));
	message_queue_->attach_logger(logger_);

	acceptor_->attach_logger(logger_);
	if (ssl_acceptor_) {
		ssl_acceptor_->attach_logger(logger_);
	}

	connection_manager_->attach_logger(logger_);
	connection_manager_->init(s);

	acceptor_->bind(s.address(), s.port(), s.backlog());
	if (ssl_acceptor_) {
		ssl_acceptor_->bind(s.ssl_address(), ssl_port, s.ssl_backlog());
	}

	for (std::vector<thread_param_type>::const_iterator i = providers_.begin(), end = providers_.end(); i != end; ++i) {
		start_provider_thread(*i);
	}

	acceptor_->accept();
	if (ssl_acceptor_) {
		ssl_acceptor_->accept();
	}
}

boost::shared_ptr<channels_stat_impl> const&
server_impl::init_channels_stat() {
	assert(!started_);
	assert(connection_manager_);
	return connection_manager_->init_channels_stat();;
}

void
server_impl::start() {
	boost::mutex::scoped_lock lock(stop_mutex_);
	assert(!started_);
	started_ = true;

	run_io();
	finish();
	if (handler_invoker_) {
		handler_invoker_->wait_for_complete();
	}
	if (connection_manager_) {
		connection_manager_->finish();
	}
	stopped_ = true;
	stop_condition_.notify_all();
}

void
server_impl::send(std::string const &to, boost::shared_ptr<message> const &m) {
	if (message_queue_ && !data_->stopping()) {
		message_queue_->send(to, m);
	}
}

void
server_impl::send(globals::connection_id to, boost::shared_ptr<message> const &m) {
	if (message_queue_ && !data_->stopping()) {
		message_queue_->send(to, m);
	}
}

void
server_impl::notify_connection_opened_failed(std::string const &to, globals::connection_id id) {

	if (!data_->stopping() && connection_manager_) {
		strand_.dispatch(boost::bind(
			&connection_manager_base::notify_connection_opened_failed, connection_manager_, to, id));
	}
}

void
server_impl::run_io() {
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
server_impl::attach_ping_formatter(boost::intrusive_ptr<ping_formatter> const &f) {
	assert(f);
	ping_formatter_ = f;
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
	catch (...) {
		logger_->error("caught unknown exception in provider function");
	}
}

}} // namespaces
