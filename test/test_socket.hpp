#pragma once

#include <thenet/socket.hpp>

namespace test
{

class Socket : public the::net::Socket
{
  public:
    Socket( int fd )
      : the::net::Socket( fd )
    {
    }

    virtual void handle_event() override
    {
    }
};

}

