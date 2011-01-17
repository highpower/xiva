#include "acsetup.hpp"
#include "python_message_filter.hpp"

#include <stdexcept>

#include "xiva/message.hpp"
#include "xiva/channel_info.hpp"

#include "interpreter_lock.hpp"

namespace xiva { namespace python {

python_message_filter::python_message_filter(py::object const &impl) :
	impl_(impl)
{
}

python_message_filter::~python_message_filter() {
}

bool
python_message_filter::allow_message(message const &msg, std::string const &channel_data) const {

	channel_info const &ch_info = msg.get_channel_info();

	try {
		interpreter_lock lock;

		return py::call_method<bool>(impl_.ptr(), "allow_message",
			ch_info.name(), ch_info.key(), ch_info.data(), channel_data);
	}
	catch (std::exception const &) {
		throw;
	}
	catch (...) {
		std::string msg = "caught unknown exception in python_message_filter::allow_message: ";
		msg.append(ch_info.name()).append("_").append(ch_info.key());
		msg.append(" ").append(ch_info.data()).append(" ").append(channel_data);
		throw std::runtime_error(msg);
	}
}

}} // namespaces
