#include "acsetup.hpp"

#include <iostream>
#include <boost/bind.hpp>
#include <boost/range.hpp>
#include <boost/function.hpp>

#include "xiva/config.hpp"
#include "xiva/message.hpp"
#include "xiva/component_set.hpp"
#include "xiva/message_sender.hpp"
#include "details/string_utils.hpp"

namespace xiva { namespace standard {

void
process_stdin_messages(message_sender &sender) {
	std::string line;
	while (sender.active() && std::getline(std::cin, line)) {
	
		try {
			typedef std::string::const_iterator iterator_type;
			typedef boost::iterator_range<iterator_type> range_type;
	
			iterator_type begin = line.begin(), end = line.end();
			iterator_type pos = std::find(begin, end, ':');
	
			if (end != pos) {
				range_type to = details::trim(range_type(begin, pos));
				range_type cont = details::trim(range_type(++pos, end));
		
				boost::shared_ptr<message> msg(new message(cont.begin(), cont.end()));
				sender.send(std::string(to.begin(), to.end()), msg);
			}
		}
		catch (std::exception const &e) {
		}
	}	
}

extern "C" XIVA_API void
xiva_register_module(settings const &s, component_set &cs) {
	(void) s;
	boost::function<void(message_sender&)> f = boost::bind(process_stdin_messages, _1);
	cs.start_provider(1, f);
}

}} // namespaces
