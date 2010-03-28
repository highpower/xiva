#include "acsetup.hpp"

#include <string>
#include <cstring>
#include <cstdlib>
#include <utility>
#include <iterator>
#include <iostream>
#include <exception>
#include <algorithm>
#include <signal.h>

#include <boost/type_traits.hpp>
#include <boost/version.hpp>

#include "details/asio.hpp"
#include "details/functors.hpp"

namespace xiva { namespace utils {

struct line_end_checker {
	template <typename Iter> std::pair<Iter, bool> operator () (Iter begin, Iter end) const;
};

}} // namespaces

XIVA_BEGIN_ASIO_NAMESPACE

template <>
struct is_match_condition<xiva::utils::line_end_checker> : public boost::true_type {
};

XIVA_END_ASIO_NAMESPACE

namespace xiva { namespace utils {

template <typename Iter> inline std::pair<Iter, bool>
line_end_checker::operator () (Iter begin, Iter end) const {
	typedef xiva::details::is_line_end<typename std::iterator_traits<Iter>::value_type> checker_type;
	Iter i = std::find_if(begin, end, checker_type());
	return std::make_pair(i, i != end);
}

void
read(asio::local::stream_protocol::socket &socket) {

	typedef std::allocator<char> char_allocator;

	syst::error_code code;
	asio::basic_streambuf<char_allocator> buf;
	std::istream stream(&buf);

	std::string line;
	line_end_checker checker;
	while (true) {
		asio::read_until(socket, buf, checker, code);
		if (code || !std::getline(stream, line)) {
			break;
		}
		std::cout << line << std::endl;
	}
}

void
start_read(std::string const &path) {

	asio::io_service io;
	asio::local::stream_protocol::socket socket(io);
	asio::local::stream_protocol::acceptor acceptor(io);
	asio::local::stream_protocol::endpoint endpoint(path);

	acceptor.open(endpoint.protocol());
	acceptor.set_option(asio::local::stream_protocol::acceptor::reuse_address(true));
	acceptor.bind(endpoint);
	acceptor.listen();

	while (true) {
		acceptor.accept(socket);
		read(socket);
		socket.close();
	}
}

void
start_write(std::string const &path) {

	asio::io_service io;
	asio::local::stream_protocol::socket socket(io);
	asio::local::stream_protocol::endpoint endpoint(path);
	socket.connect(endpoint);

	std::string line;
	while (std::getline(std::cin, line)) {
		line.push_back('\n');
		asio::write(socket, asio::buffer(line));
	}
}

std::ostream&
print_usage(std::ostream &stream) {
	stream << "Usage: xiva-util --help | [--listen] --path=<path to unix socket>" << std::endl;
	return stream;
}

}} // namespaces

int
main(int argc, char *argv[]) {
	try {
		if (2 == argc && strncmp(argv[1], "--help", sizeof("--help")) == 0) {
			xiva::utils::print_usage(std::cout);
			return EXIT_SUCCESS;
		}
		else if (2 == argc && strncmp(argv[1], "--path=", sizeof("--path=") - 1) == 0) {
			xiva::utils::start_write(argv[1] + sizeof("--path=") - 1);
			return EXIT_SUCCESS;
		}
		else if (3 == argc) {
			std::string path;
			bool listening = false;
			for (std::size_t i = 1; i < 3; ++i) {
				if (strncmp(argv[i], "--listen", sizeof("--listen")) == 0) {
					listening = true;
				}
				else if (strncmp(argv[i], "--path=", sizeof("--path=") - 1) == 0) {
					path.assign(argv[i] + sizeof("--path=") - 1);
				}
			}
			if (listening && !path.empty()) {
				signal(SIGPIPE, SIG_IGN);
				xiva::utils::start_read(path);
				return EXIT_SUCCESS;
			}
		}
		xiva::utils::print_usage(std::cerr);
		return EXIT_FAILURE;
	}
	catch (std::exception const &e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}
