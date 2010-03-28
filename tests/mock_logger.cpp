#include "acsetup.hpp"
#include "mock_logger.hpp"

namespace xiva { namespace tests {

mock_logger::mock_logger() {
}

mock_logger::~mock_logger() {
}

void
mock_logger::info(char const *format, ...) {
	(void) format;
}

void
mock_logger::debug(char const *format, ...) {
	(void) format;
}

void
mock_logger::error(char const *format, ...) {
	(void) format;
}

}} // namespaces
