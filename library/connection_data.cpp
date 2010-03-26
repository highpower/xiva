#include "acsetup.hpp"
#include "details/connection_data.hpp"

#include <fstream>
#include <stdexcept>

#include "xiva/settings.hpp"
#include "xiva/receiver_matcher.hpp"

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
		(void) e; // suppress
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

boost::intrusive_ptr<receiver_matcher> const&
connection_data::matcher() const {
	return matcher_;
}

void
connection_data::matcher(boost::intrusive_ptr<receiver_matcher> const &m) {
	assert(m);
	matcher_ = m;
}

}} // namespaces
