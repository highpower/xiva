#include "acsetup.hpp"
#include "details/websocket_info.hpp"

#include <openssl/md5.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>

#include <stdexcept>
#include <boost/cstdint.hpp>

#include "details/request_impl.hpp"
#include "details/http_constants.hpp"

#define BASE64_LENGTH(len) (4 * (((len) + 2) / 3))

namespace xiva { namespace details {

static std::string const
WS_STR_HANDSHAKE = "HTTP/1.1 101 Web Socket Protocol Handshake";

static std::string const
WS_STR_HANDSHAKE_NEW = "HTTP/1.1 101 Switching Protocols";

static std::string const
WS_STR_CONNECTION = "Connection";

static std::string const
WS_STR_UPGRADE = "Upgrade";

static std::string const
WS_STR_WEBSOCKET = "WebSocket";

static std::string const
WS_STR_WEBSOCKET_LOWERCASE = "websocket";

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

static std::string const
WS_STR_SEC_WEBSOCKET_KEY = "Sec-WebSocket-Key";

static std::string const
WS_STR_SEC_WEBSOCKET_ACCEPT = "Sec-WebSocket-Accept";

static std::string const
WS_STR_SEC_WEBSOCKET_VERSION = "Sec-WebSocket-Version";

static std::string const
WS_STR_SECRET_WORD = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

static const boost::uint8_t WS_SMALL_MSG_MAX_SIZE = 125;


websocket_info::websocket_info() :
	empty_(true), proto_78_(false)
{
}

void
websocket_info::write_headers(std::ostream &stream) const {
	if (!valid()) {
		throw std::logic_error("can not print websocket headers");
	}

	stream << (proto_78_ ? WS_STR_HANDSHAKE_NEW : WS_STR_HANDSHAKE) << http_constants<char>::endl;
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
	if (proto_78_) {
		//assert: !sec_data_.empty()
		stream << WS_STR_SEC_WEBSOCKET_ACCEPT << ": " << sec_data_;
		stream << http_constants<char>::endl;
	}
}

void
websocket_info::write_body(std::ostream &stream) const {
	if (proto_78_) {
		return;
	}
	if (!valid()) {
		throw std::logic_error("can not print websocket body");
	}
	if (!sec_data_.empty()) {
		stream.write(sec_data_.c_str(), sec_data_.size());
	}
}

bool
websocket_info::write_close(std::ostream &stream) const {
	if (!proto_78_) {
		return false;
	}
	stream << '\x88';
	return true;
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

struct cleanup_bio {
	cleanup_bio(BIO *bio) : bio_(bio) {}
	~cleanup_bio() { BIO_free_all(bio_); }

	BIO *bio_;
};

void
websocket_info::parse_sec_key(request_impl const &req) {

	std::string const &key = req.header(WS_STR_SEC_WEBSOCKET_KEY);
	if (key.empty()) {
		throw std::runtime_error("can not find key in websocket request");
	}

	enum { SHA_OUTPUT_SIZE = 20 };
	unsigned char obuf[SHA_OUTPUT_SIZE];

	std::string const key_data = key + WS_STR_SECRET_WORD;
	SHA1((const unsigned char*)key_data.c_str(), key_data.size(), obuf);

	BIO *b64 = BIO_new(BIO_f_base64());
	if (NULL == b64) {
		throw std::bad_alloc();
	}

	cleanup_bio cleaner(b64);
	BIO *bmem = BIO_new(BIO_s_mem());
	if (NULL == bmem) {
		throw std::bad_alloc();
	}

	cleaner.bio_ = b64 = BIO_push(b64, bmem);

	BIO_write(b64, obuf, sizeof(obuf));
	BIO_flush(b64);

	BUF_MEM *bptr = NULL;
        BIO_get_mem_ptr(b64, &bptr);

	if (NULL == bptr || NULL == bptr->data || bptr->length != (BASE64_LENGTH(SHA_OUTPUT_SIZE) + 1)) {
		throw std::runtime_error("can not create accept data in websocket request");
	}
	sec_data_.assign((const char*)bptr->data, bptr->length - 1);
}

void
websocket_info::parse_sec_keys(request_impl const &req) {

	std::string const &key1 = req.header(WS_STR_SEC_WEBSOCKET_KEY1);
	if (key1.empty()) {
		return;
	}
	std::string const &key2 = req.header(WS_STR_SEC_WEBSOCKET_KEY2);
	if (key2.empty()) {
		return;
	}

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

bool
websocket_info::parse(request_impl const &req, bool secure) {

	if (req.header(WS_STR_CONNECTION) != WS_STR_UPGRADE) {
		return false;
	}

	std::string const &upgrade_hdr = req.header(WS_STR_UPGRADE);
	if (upgrade_hdr == WS_STR_WEBSOCKET_LOWERCASE) {
		std::string const &version_hdr = req.header(WS_STR_SEC_WEBSOCKET_VERSION);
		if (version_hdr != "7" && version_hdr != "8") {
			throw std::runtime_error("not supported websocket protocol version: " + version_hdr);
		}
		proto_78_ = true;
	}
	else if (upgrade_hdr != WS_STR_WEBSOCKET) {
		return false;
	}
	empty_ = false;

	std::string const &host = req.header(WS_STR_HOST);
	if (host.empty()) {
		throw std::runtime_error("can not find Host in websocket request");
	}

	if (proto_78_) {
		parse_sec_key(req);
	}
	else {
		parse_sec_keys(req);
	}

	std::string const &uri = req.uri();
	if (secure) {
		location_.reserve(WS_STR_SECURE_SCHEME_DELIMITER.size() + host.size() + uri.size() + 1);
		location_.assign(WS_STR_SECURE_SCHEME_DELIMITER).append(host).append(uri);
	}
	else {
		location_.reserve(WS_STR_SCHEME_DELIMITER.size() + host.size() + uri.size() + 1);
		location_.assign(WS_STR_SCHEME_DELIMITER).append(host).append(uri);
	}

	origin_ = req.header(WS_STR_ORIGIN);
	protocol_ = req.header(WS_STR_WEBSOCKET_PROTOCOL);
	if (proto_78_) {
		size_t pos = protocol_.find(',');
		if (std::string::npos != pos) {
			protocol_.erase(pos);
		}
	}
	return true;
}

void
websocket_info::write_message(std::ostream &stream, std::string const &msg) const {

	if (!proto_78_) {
		// for UTF-8 only
		stream << '\0' << msg << '\xFF';
		return;
	}

	size_t msg_size = msg.size();
	if (msg_size <= WS_SMALL_MSG_MAX_SIZE) {
		// single text frame
		stream << '\x81' << (boost::uint8_t)msg_size << msg;
		return;
	}

	// first chunk of continuation text frame
	const char *str = msg.c_str();
	stream << '\x01' << WS_SMALL_MSG_MAX_SIZE;
	stream.write(str, WS_SMALL_MSG_MAX_SIZE);
	str += WS_SMALL_MSG_MAX_SIZE;
	msg_size -= WS_SMALL_MSG_MAX_SIZE;

	// middle chunks of continuation text frame
	while (msg_size > WS_SMALL_MSG_MAX_SIZE) {
		stream << '\x00' << WS_SMALL_MSG_MAX_SIZE;;
		stream.write(str, WS_SMALL_MSG_MAX_SIZE);
		str += WS_SMALL_MSG_MAX_SIZE;
		msg_size -= WS_SMALL_MSG_MAX_SIZE;
	}

	// last chunk of continuation text frame
	stream << '\x80' << (boost::uint8_t)msg_size;
	stream.write(str, msg_size);
}

}} // namespaces
