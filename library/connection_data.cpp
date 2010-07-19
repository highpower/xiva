#include "acsetup.hpp"
#include "details/connection_data.hpp"

#include <fstream>
#include <stdexcept>

#include "xiva/settings.hpp"
#include "xiva/formatter.hpp"
#include "xiva/response_handler.hpp"

#include "details/formatters_factory.hpp"

namespace xiva { namespace details {

connection_data::connection_data() :
	read_timeout_(0), write_timeout_(0), inactive_timeout_(0), ping_interval_(0), stopping_(false)
{
	formatters_factory_.reset(new formatters_factory());
}

connection_data::~connection_data() {
}

void
connection_data::init(settings const &s) {
	read_timeout_ = s.read_timeout();
	write_timeout_ = s.write_timeout();
	inactive_timeout_ = s.inactive_timeout();
	ping_interval_ = s.ping_interval();

	std::string policy_file_name;
	try {
		policy_file_name = s.policy_file_name();
	}
	catch (std::exception const &e) {
		// suppress
	}

	if (!policy_file_name.empty()) {
		std::ifstream is(policy_file_name.c_str(), std::ios::in);
		if (!is) {
			throw std::runtime_error("can not open policy file: " + policy_file_name);
		}
		is.exceptions(std::ios::badbit | std::ios::eofbit);

		std::streamsize size = 0;
		if (!is.seekg(0, std::ios::end)) {
			throw std::runtime_error("can not seek to end of policy file: " + policy_file_name);
		}
		size = is.tellg();
		if (!is.seekg(0, std::ios::beg)) {
			throw std::runtime_error("can not seek to begin of policy file: " + policy_file_name);
		}
		std::string data;
		data.resize(size);
		is.read(&data[0], size);

		policy_data_.swap(data);
	}
}

boost::intrusive_ptr<response_handler> const&
connection_data::handler() const {
	return handler_;
}

void
connection_data::handler(boost::intrusive_ptr<response_handler> const &h) {
	assert(h);
	handler_ = h;
}

void
connection_data::attach_logger(boost::intrusive_ptr<logger> const &log) {
	assert(log);
	logger_ = log;
}

formatters_factory const&
connection_data::fmt_factory() const {
	return *formatters_factory_;
}

formatters_factory&
connection_data::fmt_factory() {
	return *formatters_factory_;
}

bool
connection_data::stopping() const {
	return stopping_;
}

void
connection_data::stop() {
	stopping_ = true;
}

}} // namespaces
