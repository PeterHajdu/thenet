#pragma once
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>

#include <string>

namespace the
{
namespace net
{
class Socket;

void allow_address_reuse( const Socket& );
struct sockaddr_in create_base_sockaddr( int port );
void bind_to_port( int port, const Socket& );
void listen_with_queue_length( int queue_length, const Socket& );
int connect_socket( const std::string& address, int port );

}
}

