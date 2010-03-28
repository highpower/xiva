#include "acsetup.hpp"

#include <cstdlib>
#include <iostream>
#include <exception>
#include <signal.h>

#include "xiva/server.hpp"
#include "details/dynamic_loader.hpp"
#include "xml_settings.hpp"

int
main(int argc, char *argv[]) {

	try {
		xiva::details::dynamic_loader loader;
		xiva::daemon::xml_settings settings(argc, argv);
		xiva::server server;

		xiva::enumeration<std::string>::ptr_type en = settings.value_list("/xiva/modules/module");
		while (!en->empty()) {
			loader.load_module(en->next(), settings, server.components());
		}

		signal(SIGPIPE, SIG_IGN);

		server.start(settings);
		return EXIT_SUCCESS;
	}
	catch (std::exception const &e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}
