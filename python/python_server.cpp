#include "acsetup.hpp"
#include "python_server.hpp"

#include <signal.h>
#include <boost/tokenizer.hpp>

#include "xiva/message.hpp"

#include "details/server_impl.hpp"

#include "server_class.hpp"
#include "python_logger.hpp"
#include "python_handler.hpp"
#include "python_listener.hpp"
#include "python_settings.hpp"
#include "python_formatter_creator.hpp"
#include "interpreter_lock.hpp"


namespace xiva { namespace python {

python_server::python_server() :
	impl_(new details::server_impl()) 
{
	signal(SIGPIPE, SIG_IGN);
}

python_server::~python_server() {
}

void
python_server::stop() {
	if (logger_) {
		logger_->finish();
	}
	impl_->stop();
}

void
python_server::init(py::object const &impl) {
	python_settings settings(impl);
	interpreter_unlock unlock;
	impl_->init(settings);
}

void
python_server::start() {
	interpreter_thread_lock thread_lock;
	impl_->start();
}

void
python_server::send(std::string const &to, std::string const &msg) {
	boost::shared_ptr<message> m(new message(msg));
	impl_->send(to, m);
}

void
python_server::send_to_channels(std::string const &to, std::string const &msg, std::string const &channels) {
	boost::shared_ptr<message> m(new message(msg));

	if (channels.empty()) {
		return;
	}

	std::set<std::string> channels_set;

	typedef boost::char_separator<char> Separator;
	typedef boost::tokenizer<Separator> Tokenizer;
	Tokenizer tok(channels, Separator(","));
	for (Tokenizer::const_iterator it = tok.begin(), it_end = tok.end(); it != it_end; ++it) {
		std::string const &ch = *it;
		if (!ch.empty()) {
			channels_set.insert(ch);
		}
	}
	if (!channels_set.empty()) {
		m->swap_channels(channels_set);
	}
	impl_->send(to, m);
}

void
python_server::load(std::string const &name) {
	(void) name;
}

void
python_server::attach_logger(py::object const &impl) {
	boost::intrusive_ptr<python_logger> l(new python_logger(impl));
	l->start();
	impl_->attach_logger(l);
	logger_ = l;
}

void
python_server::attach_response_handler(py::object const &impl) {
	boost::intrusive_ptr<response_handler> m(new python_handler(impl));
	impl_->attach_response_handler(m);
}

void
python_server::attach_formatter_creator(std::string const &fmt_id, py::object const &impl) {
	boost::intrusive_ptr<formatter_creator> f(new python_formatter_creator(impl));
	impl_->attach_formatter_creator(fmt_id, f);
}

void
python_server::add_connection_listener(py::object const &impl) {
	boost::intrusive_ptr<connection_listener> l(new python_listener(impl));
	impl_->add_connection_listener(l);
}

void
register_server_class() throw () {
	py::class_<python_server, py::bases<>, boost::shared_ptr<python_server>, boost::noncopyable> reg("server", py::init<>());
	reg.def("stop", &python_server::stop);
	//reg.def("load", &python_server::load);
	reg.def("init", &python_server::init);
	reg.def("send", &python_server::send);
	reg.def("send_to_channels", &python_server::send_to_channels);
	reg.def("start", &python_server::start);
	reg.def("attach_logger", &python_server::attach_logger);
	reg.def("add_connection_listener", &python_server::add_connection_listener);
	reg.def("attach_response_handler", &python_server::attach_response_handler);
	reg.def("attach_formatter_creator", &python_server::attach_formatter_creator);
}

}} // namespaces
