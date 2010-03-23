#include "acsetup.hpp"
#include "details/connection_data.hpp"

#include "xiva/settings.hpp"
#include "xiva/receiver_matcher.hpp"
//#include "details/connection_manager.hpp"

namespace xiva { namespace details {

connection_data::connection_data() :
	read_timeout_(0), write_timeout_(0), inactive_timeout_(0)
{
}

connection_data::~connection_data() {
}

void
connection_data::init(settings const &s) {
	//manager_->init(s);
	read_timeout_ = s.read_timeout();
	write_timeout_ = s.write_timeout();
	inactive_timeout_ = s.inactive_timeout();
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

/*
boost::intrusive_ptr<connection_manager> const&
connection_data::manager() const {
	return manager_;
}

void
connection_data::manager(boost::intrusive_ptr<connection_manager> const &m) {
	assert(m);
	manager_ = m;
}
 */

}} // namespaces
