#include "acsetup.hpp"
#include "details/ssl_connection_socket.hpp"

namespace xiva { namespace details {

ssl_connection_socket::ssl_connection_socket(asio::io_service &io, asio::ssl::context& context) : socket_(io, context)
{
}

ssl_connection_socket::~ssl_connection_socket() {
}

}} // namespaces
