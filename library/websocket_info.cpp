#include "acsetup.hpp"

#include <stdexcept>

#include "xiva/request.hpp"
#include "details/websocket_info.hpp"
#include "details/http_constants.hpp"


namespace xiva { namespace details {


static std::string const
WS_STR_CONNECTION = "Connection";

static std::string const
WS_STR_UPGRADE = "Upgrade";

static std::string const
WS_STR_WEBSOCKET = "WebSocket";

static std::string const
WS_STR_ORIGIN = "Origin";

static std::string const
WS_STR_HOST = "Host";

static std::string const
WS_STR_PROTOCOL = "WebSocket-Protocol";

static std::string const
WS_STR_LOCATION = "WebSocket-Location";


websocket_info::websocket_info() {
}


std::ostream&
operator << (std::ostream &stream, websocket_info const &val) {

	stream << "HTTP/1.1 101 Web Socket Protocol Handshake" << http_constants::endl;
	stream << WS_STR_UPGRADE << ": " << WS_STR_WEBSOCKET << http_constants::endl;
	stream << WS_STR_CONNECTION << ": " << WS_STR_UPGRADE << http_constants::endl;

        if (!val.origin_.empty()) {
		stream << WS_STR_WEBSOCKET << "-" << WS_STR_ORIGIN << ": " << val.origin_;
		stream << http_constants::endl;
	}

	stream << WS_STR_LOCATION << ": " << val.location_ << http_constants::endl;

        if (!val.protocol_.empty()) {
		stream << WS_STR_PROTOCOL << ": " << val.protocol_;
		stream << http_constants::endl;
	}
	return stream;
}

void
websocket_info::parse(request const &req) {
	if (req.header(WS_STR_CONNECTION) != WS_STR_UPGRADE || req.header(WS_STR_UPGRADE) != WS_STR_WEBSOCKET) {
		return;
	}

	std::string const &host = req.header(WS_STR_HOST);
	if (host.empty()) {
		throw std::runtime_error("can not find Host in websocket request");
	}
        location_.assign("ws://").append(host).append(req.url());

        origin_ = req.header(WS_STR_ORIGIN);
        protocol_ = req.header(WS_STR_PROTOCOL);
}
	
void
websocket_info::write_message(std::ostream &stream, std::string const &msg) {
	// for UTF-8 only
	stream << '\0' << msg << '\xFF';
}


}} // namespaces
