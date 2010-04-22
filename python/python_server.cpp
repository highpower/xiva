#include "acsetup.hpp"
#include "python_server.hpp"

#include <boost/bind.hpp>

#include "server_class.hpp"
#include "python_logger.hpp"
#include "python_handler.hpp"
#include "python_listener.hpp"
#include "python_settings.hpp"
#include "interpreter_lock.hpp"

#include "xiva/message.hpp"

#include "details/server_impl.hpp"

namespace xiva { namespace python {

python_server::python_server() :
	impl_(new details::server_impl()) 
{
}

python_server::~python_server() {
}

void
python_server::stop() {
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
	//interpreter_unlock unlock;
	//impl_->start();
	boost::function<void()> func = boost::bind(&details::server_impl::start, impl_.get());
	interpreter_init::async_exec(func);
}

void
python_server::send(std::string const &to, std::string const &msg) {
	boost::shared_ptr<message> m(new message(msg));
	impl_->send(to, m);
}

void
python_server::load(std::string const &name) {
}

void
python_server::attach_logger(py::object const &impl) {
	boost::intrusive_ptr<logger> l(new python_logger(impl));
	impl_->attach_logger(l);
}

void
python_server::attach_response_handler(py::object const &impl) {
	boost::intrusive_ptr<response_handler> m(new python_handler(impl));
	impl_->attach_response_handler(m);
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
	reg.def("load", &python_server::load);
	reg.def("init", &python_server::init);
	reg.def("send", &python_server::send);
	reg.def("start", &python_server::start);
	reg.def("attach_logger", &python_server::attach_logger);
	reg.def("add_connection_listener", &python_server::add_connection_listener);
	reg.def("attach_response_handler", &python_server::attach_response_handler);
}

}} // namespaces
