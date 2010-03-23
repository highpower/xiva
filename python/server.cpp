#include "acsetup.hpp"

#include "server.hpp"
#include "server_class.hpp"
#include "python_logger.hpp"
#include "python_matcher.hpp"
#include "python_listener.hpp"
#include "python_settings.hpp"
#include "interpreter_lock.hpp"

#include "details/server_impl.hpp"

namespace xiva { namespace python {

server::server() :
	impl_(new details::server_impl())
{
}

void
server::stop() {
	impl_->stop();
}

void
server::start(py::object const &impl) {
	python_settings settings(impl);
	interpreter_unlock unlock;
	impl_->start(settings);
}

void
server::load(std::string const &name) {
}

void
server::attach_logger(py::object const &impl) {
	boost::intrusive_ptr<logger> l(new python_logger(impl));
	impl_->attach_logger(l);
}

void
server::attach_receiver_matcher(py::object const &impl) {
	boost::intrusive_ptr<receiver_matcher> m(new python_matcher(impl));
	impl_->attach_receiver_matcher(m);
}

void
server::add_connection_listener(py::object const &impl) {
	boost::intrusive_ptr<connection_listener> l(new python_listener(impl));
	impl_->add_connection_listener(l);
}

void
register_server_class() throw () {
	py::class_<server, py::bases<>, server*, boost::noncopyable> reg("server");
	reg.def("stop", &server::stop);
	reg.def("load", &server::load);
	reg.def("start", &server::start);
}

}} // namespaces
