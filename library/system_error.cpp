#include "acsetup.hpp"
#include "details/system_error.hpp"

#include <cerrno>
#include <cstring>

namespace xiva { namespace details {

inline char const*
wrap_strerror(char const *buffer, char const *) {
	return buffer;
}

inline char const*
wrap_strerror(int, char const *buffer) {
	return buffer;
}

system_error::system_error(char const *message) :
	error("%s", message)
{
}

system_error::~system_error() throw () {
}

void
system_error::throw_error(int error) {
	char buffer[256];
	char const *res = wrap_strerror(strerror_r(error, buffer, sizeof(buffer)), buffer);
	throw system_error(res);
}

void
system_error::throw_unless(bool condition) {
	if (!condition) {
		throw_error(errno);
	}
}

}} // namespaces
