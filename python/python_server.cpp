#include "acsetup.hpp"
#include "python_server.hpp"

#include <stdexcept>
#include <signal.h>
#include <boost/python/errors.hpp>

#include "xiva/message.hpp"

#include "details/channels_stat_impl.hpp"
#include "details/server_impl.hpp"

#include "server_class.hpp"
#include "python_logger.hpp"
#include "python_handler.hpp"
#include "python_listener.hpp"
#include "python_message_filter.hpp"
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
	try {
		check_server();
		impl_->stop();
	}
	catch (std::exception const &e) {
		PyErr_SetString(PyExc_RuntimeError, e.what());
		boost::python::throw_error_already_set();
	}
}

void
python_server::init(py::object const &impl) {
	try {
		check_server();
		python_settings settings(impl);
		interpreter_thread_lock thread_lock;
		impl_->init(settings);
	}
	catch (std::exception const &e) {
		PyErr_SetString(PyExc_RuntimeError, e.what());
		boost::python::throw_error_already_set();
	}
}

void
python_server::list_channels_enable() {
	try {
		check_server();
		channels_stat_ = impl_->init_channels_stat();
	}
	catch (std::exception const &e) {
		PyErr_SetString(PyExc_RuntimeError, e.what());
		boost::python::throw_error_already_set();
	}
}

void
python_server::start() {
	try {
		check_server();
		{
			interpreter_thread_lock thread_lock;
			cleanup_list_auto cleanup_auto(cleanup_);
			logger_->start();
			impl_->start();
		}
		impl_.reset();
		channels_stat_.reset();
		logger_ = NULL;
	}
	catch (std::exception const &e) {
		PyErr_SetString(PyExc_RuntimeError, e.what());
		boost::python::throw_error_already_set();
	}
}

py::list
python_server::list_channels() const {

	py::list result;
	try {
		if (channels_stat_) {
			//interpreter_unlock unlock;
			enumeration<std::string>::ptr_type en = channels_stat_->load_names();
			while (!en->empty()) {
				result.append(en->next());
			}
		}
	}
	catch (std::exception const &e) {
		PyErr_SetString(PyExc_RuntimeError, e.what());
		boost::python::throw_error_already_set();
	}
	return result;
}

py::list
python_server::list_channel(std::string const &channel_name) const {

	py::list result;
	try {
		if (channels_stat_) {
			//interpreter_unlock unlock;
			enumeration<std::string>::ptr_type en = channels_stat_->load_keys(channel_name);
			while (!en->empty()) {
				result.append(en->next());
			}
		}
	}
	catch (std::exception const &e) {
		PyErr_SetString(PyExc_RuntimeError, e.what());
		boost::python::throw_error_already_set();
	}
	return result;
}

void
python_server::send(std::string const &to, std::string const &msg) {

	try {
		if (impl_) {
			//interpreter_unlock unlock;
			boost::shared_ptr<message> m(new message(msg));
			impl_->send(to, m);
		}
	}
	catch (std::exception const &e) {
		PyErr_SetString(PyExc_RuntimeError, e.what());
		boost::python::throw_error_already_set();
	}
}

void
python_server::send_to_channel(
		std::string const &to, std::string const &msg,
		std::string const &channel_name, std::string const &channel_key, std::string const &channel_data) {

	try {
		if (impl_) {
			//interpreter_unlock unlock;
			boost::shared_ptr<message> m(new message(msg));
			channel_info ch_info(channel_name, channel_key, channel_data);
			if (!ch_info.empty()) {
				m->set_channel_info(ch_info);
				impl_->send(to, m);
			}
		}
	}
	catch (std::exception const &e) {
		PyErr_SetString(PyExc_RuntimeError, e.what());
		boost::python::throw_error_already_set();
	}
}

//void
//python_server::load(std::string const &name) {
//	(void) name;
//}

void
python_server::attach_logger(py::object const &impl) {

	try {
		check_server();
		boost::intrusive_ptr<python_logger> l(new python_logger(impl));
		impl_->attach_logger(l);
		logger_ = l;
	}
	catch (std::exception const &e) {
		PyErr_SetString(PyExc_RuntimeError, e.what());
		boost::python::throw_error_already_set();
	}
}

void
python_server::attach_response_handler(py::object const &impl) {
	try {
		check_server();
		boost::intrusive_ptr<response_handler> m(new python_handler(impl));
		impl_->attach_response_handler(m);
	}
	catch (std::exception const &e) {
		PyErr_SetString(PyExc_RuntimeError, e.what());
		boost::python::throw_error_already_set();
	}
}

void
python_server::attach_formatter_creator(std::string const &fmt_id, py::object const &impl) {

	try {
		check_server();
		boost::intrusive_ptr<formatter_creator> f(new python_formatter_creator(impl, cleanup_));
		impl_->attach_formatter_creator(fmt_id, f);
	}
	catch (std::exception const &e) {
		PyErr_SetString(PyExc_RuntimeError, e.what());
		boost::python::throw_error_already_set();
	}
}

void
python_server::add_connection_listener(py::object const &impl) {

	try {
		check_server();
		boost::intrusive_ptr<connection_listener> l(new python_listener(impl));
		impl_->add_connection_listener(l);
	}
	catch (std::exception const &e) {
		PyErr_SetString(PyExc_RuntimeError, e.what());
		boost::python::throw_error_already_set();
	}
}

void
python_server::attach_message_filter(py::object const &impl) {

	try {
		check_server();
		boost::intrusive_ptr<message_filter> f(new python_message_filter(impl));
		impl_->attach_message_filter(f);
	}
	catch (std::exception const &e) {
		PyErr_SetString(PyExc_RuntimeError, e.what());
		boost::python::throw_error_already_set();
	}
}

void
python_server::check_server() const {
	if (!impl_) {
		throw std::runtime_error("server already stopped");
	}
}


void
register_server_class() throw () {
	py::class_<python_server, py::bases<>, boost::shared_ptr<python_server>, boost::noncopyable> reg("server", py::init<>());
	reg.def("stop", &python_server::stop);
	//reg.def("load", &python_server::load);
	reg.def("init", &python_server::init);
	reg.def("send", &python_server::send);
	reg.def("send_to_channel", &python_server::send_to_channel);
	reg.def("start", &python_server::start);
	reg.def("attach_logger", &python_server::attach_logger);
	reg.def("add_connection_listener", &python_server::add_connection_listener);
	reg.def("attach_response_handler", &python_server::attach_response_handler);
	reg.def("attach_formatter_creator", &python_server::attach_formatter_creator);
	reg.def("attach_message_filter", &python_server::attach_message_filter);
	reg.def("list_channels_enable", &python_server::list_channels_enable);
	reg.def("list_channels", &python_server::list_channels);
	reg.def("list_channel", &python_server::list_channel);
}

}} // namespaces
