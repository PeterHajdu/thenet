#include <thenet/socket.hpp>
#include <unistd.h>

namespace the
{
namespace net
{
Socket::Socket( int fd )
  : fd( fd )
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

