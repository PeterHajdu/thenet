#include <thenet/socket_utils.hpp>
#include <thenet/socket.hpp>
#include <thenet/address.hpp>

#include <sys/types.h>
#include <sys/socket.h>

#include <string>
#include <cstring>

namespace the
{
namespace net
{

void allow_address_reuse( const Socket& socket )
{
  const int true_value( 1 );
  setsockopt(
      socket.fd,
      SOL_SOCKET,
      SO_REUSEADDR,
      &true_value,
      sizeof( true_value ) );
}


sockaddr_in create_base_sockaddr( int port )
{
  struct sockaddr_in address;
  memset( &address, 0, sizeof( address ) );
  address.sin_family = AF_INET;
  address.sin_port = htons( port );
  return address;
}


void bind_to_port( int port, const Socket& socket )
{
  struct sockaddr_in address( create_base_sockaddr( port ) );
  address.sin_addr.s_addr = INADDR_ANY;

  bind( socket.fd, (struct sockaddr *) &address, sizeof( address ) );
}


void listen_with_queue_length( int queue_length, const Socket&  socket )
{
  ::listen( socket.fd, queue_length );
}


int connect_socket( const Address& address )
{
  struct hostent *serverHost( gethostbyname( address.host.c_str() ) );
  if ( nullptr == serverHost )
  {
    return -1;
  }

  struct sockaddr_in serverData( create_base_sockaddr( address.port ) );
  serverData.sin_family = AF_INET;
  memcpy(  &(serverData.sin_addr.s_addr), serverHost->h_addr, serverHost->h_length );
  const int new_socket( socket( PF_INET, SOCK_STREAM, IPPROTO_TCP ) );
  if ( ::connect( new_socket, (struct sockaddr *)&serverData, sizeof( serverData ) ) < 0 )
  {
    return -1;
  }

  return new_socket;
}


}
}


