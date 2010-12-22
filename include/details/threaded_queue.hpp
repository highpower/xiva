// xiva (acronym for HTTP Extended EVent Automata) is a simple HTTP server.
// Copyright (C) 2009 Yandex <highpower@yandex.ru>

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef XIVA_DETAILS_THREADED_QUEUE_HPP_INCLUDED
#define XIVA_DETAILS_THREADED_QUEUE_HPP_INCLUDED

#include <deque>
#include <functional>
#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>

namespace xiva { namespace details {

template <typename Item>
class threaded_queue {

public:
	threaded_queue();
	virtual ~threaded_queue();

	bool pop(Item &item);
	bool push(Item const &item);
	void finish();

	bool finished() const;

	typedef std::deque<Item> raw_items_type;

	bool pop_all(std::deque<Item> &data);
	void swap_data(std::deque<Item> &data);

private:
	threaded_queue(threaded_queue const &);
	threaded_queue& operator = (threaded_queue const &);

private:
	volatile bool finished_;
	mutable boost::mutex mutex_;
	boost::condition condition_;
	std::deque<Item> items_;
};

template <typename Item> inline
threaded_queue<Item>::threaded_queue() :
	finished_(false)
{
}

template <typename Item> inline
threaded_queue<Item>::~threaded_queue() {
}

template <typename Item> inline bool
threaded_queue<Item>::pop(Item &item) {
	boost::mutex::scoped_lock lock(mutex_);
	while (!finished_ && items_.empty()) {
		condition_.wait(lock);
	}
	if (finished_) {
		return false;
	}
	Item i = items_.front();
	items_.pop_front();
	std::swap(i, item);
	return true;
}

template <typename Item> inline bool
threaded_queue<Item>::pop_all(std::deque<Item> &data) {
	data.clear();
	boost::mutex::scoped_lock lock(mutex_);
	while (!finished_ && items_.empty()) {
		condition_.wait(lock);
	}
	if (finished_) {
		return false;
	}
	items_.swap(data);
	return true;
}

template <typename Item> inline bool
threaded_queue<Item>::push(Item const &item) {
	boost::mutex::scoped_lock lock(mutex_);
	if (finished_) {
		return false;
	}
	items_.push_back(item);
	condition_.notify_all();
	return true;
}

template <typename Item> inline void
threaded_queue<Item>::finish() {
	boost::mutex::scoped_lock lock(mutex_);
	finished_ = true;
	condition_.notify_all();
}

template <typename Item> inline bool
threaded_queue<Item>::finished() const {
	boost::mutex::scoped_lock lock(mutex_);
	return finished_;
}

template <typename Item> inline void
threaded_queue<Item>::swap_data(std::deque<Item> &data) {
	boost::mutex::scoped_lock lock(mutex_);
	items_.swap(data);
}

}} // namespaces

#endif // XIVA_DETAILS_THREADED_QUEUE_HPP_INCLUDED
