#include "acsetup.hpp"

#include <boost/ref.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/thread/thread.hpp>
#include <boost/test/unit_test.hpp>

#include "details/threaded_queue.hpp"

namespace xiva { namespace tests {

using namespace details;

BOOST_AUTO_TEST_SUITE(queue_test)

void
queue_thread_push_func(threaded_queue<std::size_t> &queue) {
	for (std::size_t i = 0; i < 100; ++i) {
		queue.push(i);
	}
	queue.finish();
}

BOOST_AUTO_TEST_CASE(test_queue_push) {

	threaded_queue<std::size_t> queue;
	boost::function<void()> f = boost::bind(&queue_thread_push_func, boost::ref(queue));
	boost::thread thread(f);

	std::size_t item = 0;
	for (std::size_t i = 0; queue.pop(item); ++i) {
		BOOST_CHECK_EQUAL(i, item);
	}
	thread.join();
}

BOOST_AUTO_TEST_SUITE_END()

}} // namespaces
