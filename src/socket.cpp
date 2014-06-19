#include <thenet/socket.hpp>
#include <unistd.h>
#include <fcntl.h>

namespace
{
  int last_id{ 0 };
  int next_socket_id()
  {
    return last_id++;
  }
}

namespace the
{
namespace net
{
Socket::Socket( int fd )
  : fd( fd )
  , id( next_socket_id() )
{
}



size_t
Socket::send( const char* message, size_t length )
{
  const ssize_t sent_bytes( ::write( fd, message, length ) );
  return sent_bytes;
}

Socket::~Socket()
{
  close( fd );
}

void
set_non_blocking( const Socket& socket )
{
  const int flags( fcntl( socket.fd, F_GETFL, 0 ) );
  fcntl( socket.fd, F_SETFL, flags | O_NONBLOCK );
}

}
}

