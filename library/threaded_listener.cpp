#include "acsetup.hpp"
#include "details/threaded_listener.hpp"

#include <cassert>

#include <boost/bind.hpp>
#include <boost/crc.hpp>
#include <boost/function.hpp>

#include "xiva/settings.hpp"
#include "details/connection_data.hpp"

namespace xiva { namespace details {

threaded_listener::threaded_listener(connection_data const &data) : data_(data)
{
}

threaded_listener::~threaded_listener() {
	try {
		finish();
		join_all();
	}
	catch (std::exception const &e) {
		(void) e;
	}
}

void
threaded_listener::thread_func(unsigned int index) {

	boost::shared_ptr<queue_type> items_by_index = items_[index];

	typedef queue_type::raw_items_type items_type;
	items_type items;

	while (items_by_index->pop_all(items)) {
		for (items_type::iterator i = items.begin(), end = items.end(); i != end; ++i) {
			process_item(*i);
		}
	}
}

void
threaded_listener::process_item(queue_item_type &item) {
	try {
		if (!item.notify_guard) {
			notify_disconnected(item.to);
		}
		else if (item.for_open) {
			if (item.notify_guard->open()) { // open once
				try {
					notify_connection_opened(item.to, item.id);
					item.notify_guard->commit(); // allow notify close for next event
				}
				catch (...) {
					item.notify_guard->cancel(); // disallow notify close for next event
					data_.notify_connection_opened_failed(item.to, item.id);
				}
			}
		}
		else if (item.notify_guard->close()) { // close once (if not canceled)
			notify_connection_closed(item.to, item.id);
		}
	}
	catch (...) {
	}
}

void
threaded_listener::init(settings const &s) {
	if (!empty()) {
		unsigned short threads = s.listener_threads();
		if (!threads) {
			threads = 1;
		}
		for (unsigned int i = 0; i < threads; ++i) {
			items_.push_back(boost::shared_ptr<queue_type>(new queue_type()));
			boost::function<void()> f = boost::bind(&threaded_listener::thread_func, this, i);
			create_thread(f);
		}
	}
}

void
threaded_listener::finish() {
	for(unsigned int i = 0; i < items_.size(); ++i) {
		items_[i]->finish();
	}
}

unsigned int
threaded_listener::calc_index(std::string const &to) const {

	std::size_t size = items_.size();
	assert(size);

	if (1 == size) {
		return 0;
	}

	boost::crc_32_type result;
	result.process_bytes(to.c_str(), to.length());
	return static_cast<unsigned int>(result.checksum() % size);
}

void
threaded_listener::connection_opened_ex(
	std::string const &to, globals::connection_id id,
	boost::shared_ptr<guard> const &notify_guard) throw (std::exception) {

	assert(NULL != notify_guard.get());
	if (!empty()) {
		unsigned int index = calc_index(to);

		queue_item_type item;
		item.to = to;
		item.id = id;
		item.notify_guard = notify_guard;
		item.for_open = true;
		items_[index]->push(item);
	}
}

void
threaded_listener::connection_closed_ex(
	std::string const &to, globals::connection_id id,
	boost::shared_ptr<guard> const &notify_guard) throw (std::exception) {

	assert(NULL != notify_guard.get());
	if (!empty()) {
		unsigned int index = calc_index(to);

		queue_item_type item;
		item.to = to;
		item.id = id;
		item.notify_guard = notify_guard;
		item.for_open = false;
		items_[index]->push(item);
	}
}

void
threaded_listener::connection_opened(std::string const &to, globals::connection_id id) throw (std::exception) {
	(void) to;
	(void) id;
	assert(false);
}

void
threaded_listener::connection_closed(std::string const &to, globals::connection_id id) throw (std::exception) {
	(void) to;
	(void) id;
	assert(false);
}

void
threaded_listener::disconnected(std::string const &to) throw (std::exception) {
	if (!empty()) {
		unsigned int index = calc_index(to);

		queue_item_type item;
		item.to = to;
		item.id = 0;
		items_[index]->push(item);
	}
}

}} // namespaces
