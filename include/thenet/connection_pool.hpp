#pragma once

namespace the
{
namespace net
{
class Socket;

class ConnectionPool
{
  public:
    void on_new_socket( Socket& new_socket );
};

}
}

