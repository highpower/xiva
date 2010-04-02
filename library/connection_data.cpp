#include "acsetup.hpp"
#include "details/connection_data.hpp"

#include <fstream>
#include <stdexcept>

#include "xiva/settings.hpp"
#include "xiva/formatter.hpp"
#include "xiva/response_handler.hpp"

namespace xiva { namespace details {

connection_data::connection_data() :
	read_timeout_(0), write_timeout_(0), inactive_timeout_(0)
{
}

connection_data::~connection_data() {
}

void
connection_data::init(settings const &s) {
	read_timeout_ = s.read_timeout();
	write_timeout_ = s.write_timeout();
	inactive_timeout_ = s.inactive_timeout();

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

formatter const*
connection_data::find_formatter(std::string const &fmt_id) const {
	formatters_type::const_iterator i = formatters_.find(fmt_id);
	if (formatters_.end() != i) {
		return i->second.get();
	}
	return NULL;
}

void
connection_data::attach_formatter(std::string const &fmt_id, boost::intrusive_ptr<formatter> const &fmt_ptr) {
	formatters_.insert(std::make_pair(fmt_id, fmt_ptr));
}

}} // namespaces
