#pragma once

#include <memory>

namespace the
{
namespace net
{

//todo: use template parameter for polymorphism
class Socket
{
  public:
    typedef std::unique_ptr< Socket > Pointer;
    Socket( int fd );
    Socket( const Socket& ) = delete;
    Socket& operator=( const Socket& ) = delete;

    virtual ~Socket();

    size_t send( const char* message, size_t length );

    const int fd;
    virtual void handle_event() = 0;
};
}
}

