#include "acsetup.hpp"
#include "details/ssl_context.hpp"

#include <string>

#include "xiva/settings.hpp"
#include "details/ssl_connection_socket.hpp"


namespace xiva { namespace details {

ssl_context::ssl_context(asio::io_service &io) : context_(io, asio::ssl::context::sslv23)
{
}

ssl_context::~ssl_context() {
}

void
ssl_context::init(settings const &s) {

	context_.set_options(asio::ssl::context::default_workarounds);

	std::string cacert = s.ssl_cacert_file_name();
	if (!cacert.empty()) {
		context_.load_verify_file(cacert.c_str());
	}

	std::string cert = s.ssl_cert_file_name();
	if (!cert.empty()) {
		//context_.use_certificate_file(cert.c_str(), asio::ssl::context::pem);
		context_.use_certificate_chain_file(cert.c_str());
		context_.use_private_key_file(cert.c_str(), asio::ssl::context::pem);
	}
}

std::auto_ptr<ssl_connection_socket>
ssl_context::create_socket(asio::io_service &io) {
	return std::auto_ptr<ssl_connection_socket>(new ssl_connection_socket(io, context_));
}

}} // namespaces
