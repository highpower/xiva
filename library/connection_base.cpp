#include "acsetup.hpp"

#include <boost/static_assert.hpp>

#include "details/connection_base.hpp"

namespace xiva { namespace details {

connection_base::connection_base()
{
}

connection_base::~connection_base() {
}

globals::connection_id
connection_base::id() const {
	BOOST_STATIC_ASSERT(sizeof(globals::connection_id) == sizeof(connection_base const*));
	return reinterpret_cast<globals::connection_id>(this);
}

}} // namespaces
