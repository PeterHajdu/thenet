#include <thenet/socket.hpp>
#include <unistd.h>

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
  return ::write( fd, message, length );
}

Socket::~Socket()
{
  close( fd );
}
}
}

