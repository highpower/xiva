#include "acsetup.hpp"
#include "details/threaded_listener.hpp"

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "xiva/settings.hpp"

namespace xiva { namespace details {

threaded_listener::threaded_listener()
{
}

threaded_listener::~threaded_listener() {
	try {
		items_.finish();
		join_all();
	}
	catch (std::exception const &e) {
	}
}

void
threaded_listener::thread_func() {
	queue_item_type item;
	while (items_.pop(item)) {
		if (item.second) {
			notify_connection_opened(item.first);
		}
		else {
			notify_connection_closed(item.first);
		}
	}
}

void
threaded_listener::init(settings const &s) {
	boost::function<void()> f = boost::bind(&threaded_listener::thread_func, this);
	unsigned short threads = s.listener_threads();
	for (unsigned short i = 0; i < threads; ++i) {
		create_thread(f);
	}
}

void
threaded_listener::connection_opened(std::string const &to) throw (std::exception) {
	items_.push(queue_item_type(to, true));
}

void
threaded_listener::connection_closed(std::string const &to) throw (std::exception) {
	items_.push(queue_item_type(to, false));
}

}} // namespaces
