#include "acsetup.hpp"
#include "xiva/receiver_matcher.hpp"

namespace xiva {

receiver_matcher::receiver_matcher()
{
}

receiver_matcher::~receiver_matcher() {
}

void
receiver_matcher::check(request const &req) const {
	(void) req;
}

} // namespace
