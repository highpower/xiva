#include "acsetup.hpp"
#include "details/websocket_info.hpp"

#include <openssl/md5.h>

#include <stdexcept>
#include <boost/cstdint.hpp>

#include "details/request_impl.hpp"
#include "details/http_constants.hpp"

namespace xiva { namespace details {

static std::string const
WS_STR_HANDSHAKE = "HTTP/1.1 101 Web Socket Protocol Handshake";

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
WS_STR_WEBSOCKET_ORIGIN = "WebSocket-Origin";

static std::string const
WS_STR_WEBSOCKET_PROTOCOL = "WebSocket-Protocol";

static std::string const
WS_STR_WEBSOCKET_LOCATION = "WebSocket-Location";

static std::string const
WS_STR_SCHEME_DELIMITER = "ws://";

static std::string const
WS_STR_SECURE_SCHEME_DELIMITER = "wss://";

static std::string const
WS_STR_SEC_PREFIX = "Sec-";

static std::string const
WS_STR_SEC_WEBSOCKET_KEY1 = "Sec-WebSocket-Key1";

static std::string const
WS_STR_SEC_WEBSOCKET_KEY2 = "Sec-WebSocket-Key2";

websocket_info::websocket_info() : 
empty_(true)
{
}

void
websocket_info::write_headers(std::ostream &stream) const {
	if (!valid()) {
		throw std::logic_error("can not print websocket headers");
	}

	stream << WS_STR_HANDSHAKE << http_constants<char>::endl;
	stream << WS_STR_UPGRADE << ": " << WS_STR_WEBSOCKET << http_constants<char>::endl;
	stream << WS_STR_CONNECTION << ": " << WS_STR_UPGRADE << http_constants<char>::endl;

	if (!origin_.empty()) {
		if (!sec_data_.empty()) {
			stream << WS_STR_SEC_PREFIX;
		}
		stream << WS_STR_WEBSOCKET_ORIGIN << ": " << origin_;
		stream << http_constants<char>::endl;
	}

	if (!sec_data_.empty()) {
		stream << WS_STR_SEC_PREFIX;
	}
	stream << WS_STR_WEBSOCKET_LOCATION << ": " << location_ << http_constants<char>::endl;

	if (!protocol_.empty()) {
		if (!sec_data_.empty()) {
			stream << WS_STR_SEC_PREFIX;
		}
		stream << WS_STR_WEBSOCKET_PROTOCOL << ": " << protocol_;
		stream << http_constants<char>::endl;
	}
}

void
websocket_info::write_body(std::ostream &stream) const {
	if (!valid()) {
		throw std::logic_error("can not print websocket body");
	}
	if (!sec_data_.empty()) {
		stream.write(sec_data_.c_str(), sec_data_.size());
	}
}
		
static boost::int32_t
get_websocket_key_value(char const *s, std::string const &key) {
	boost::int64_t v = 0;
	boost::int32_t ac = 0;
	for (const char *pa = s; *pa; ++pa) {
		if (*pa == ' ') {
			ac++;
		}
		else if (*pa >= '0' && *pa <= '9') {
			v = v * 10 + (*pa - '0');
		}
	}
	if (!ac) {
		throw std::runtime_error(("invalid http header " + key + " : ") + s);
	}

	boost::int32_t v32 = (boost::int32_t)(v / ac);

	boost::uint8_t *b = (uint8_t*)&v32;
	uint8_t t = b[0];
	b[0] = b[3];
	b[3] = t;
	t = b[1];
	b[1] = b[2];
	b[2] = t;
	return v32;
}

void
websocket_info::parse(request_impl const &req, bool secure) {

	if (req.header(WS_STR_CONNECTION) != WS_STR_UPGRADE || req.header(WS_STR_UPGRADE) != WS_STR_WEBSOCKET) {
		return;
	}
	empty_ = false;

	std::string const &host = req.header(WS_STR_HOST);
	if (host.empty()) {
		throw std::runtime_error("can not find Host in websocket request");
	}

	std::string const &key1 = req.header(WS_STR_SEC_WEBSOCKET_KEY1);
	std::string const &key2 = req.header(WS_STR_SEC_WEBSOCKET_KEY2);
	if (!key1.empty() && !key2.empty()) {
		if (req.body().size() < 8) {
			throw std::runtime_error("can not find data in websocket request");
		}
		boost::int32_t v1 = get_websocket_key_value(key1.c_str(), WS_STR_SEC_WEBSOCKET_KEY1);
		boost::int32_t v2 = get_websocket_key_value(key2.c_str(), WS_STR_SEC_WEBSOCKET_KEY2);

		MD5_CTX md5handler;
		MD5_Init(&md5handler);
		MD5_Update(&md5handler, &v1, sizeof(v1));
		MD5_Update(&md5handler, &v2, sizeof(v2));
		MD5_Update(&md5handler, req.body().data(), 8);

		std::string str;
		str.resize(16);
		MD5_Final((unsigned char*)&str[0], &md5handler);

		str.swap(sec_data_);
	}

	std::string const &uri = req.uri();
	if (secure) {
		location_.reserve(WS_STR_SCHEME_DELIMITER.size() + host.size() + uri.size() + 1);
		location_.assign(WS_STR_SCHEME_DELIMITER).append(host).append(uri);
	}
	else {
		location_.reserve(WS_STR_SECURE_SCHEME_DELIMITER.size() + host.size() + uri.size() + 1);
		location_.assign(WS_STR_SECURE_SCHEME_DELIMITER).append(host).append(uri);
	}

	origin_ = req.header(WS_STR_ORIGIN);
	protocol_ = req.header(WS_STR_WEBSOCKET_PROTOCOL);
}

void
websocket_info::write_message(std::ostream &stream, std::string const &msg) {
	// for UTF-8 only
	stream << '\0' << msg << '\xFF';
}


}} // namespaces
