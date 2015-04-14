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
    Socket( Socket&& ) = delete;
    Socket& operator=( Socket&& ) = delete;

    virtual ~Socket();

    virtual void drop() = 0;
    size_t send( const char* message, size_t length );
    void close();

    const int fd;
    const int id;

    virtual void handle_event() = 0;
};

void set_non_blocking( const Socket& );

}
}

