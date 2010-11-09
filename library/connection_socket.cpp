#include "acsetup.hpp"
#include "details/connection_socket.hpp"

namespace xiva { namespace details {

connection_socket::connection_socket(asio::io_service &io) : socket_(io)
{
}

connection_socket::~connection_socket() {
}

}} // namespaces
