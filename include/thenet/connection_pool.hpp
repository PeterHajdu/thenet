#pragma once

#include <functional>
#include <unordered_map>

#include <thenet/connection.hpp>

namespace the
{
namespace net
{
class Socket;

class ConnectionPool
{
  public:
    typedef std::function<void( Connection& )> ConnectionEventCallback;
    ConnectionPool(
        ConnectionEventCallback new_connection_callback,
        ConnectionEventCallback connection_lost_callback );

    void on_new_socket( Socket& new_socket );
    void on_socket_lost( Socket& lost_socket );

  private:
    ConnectionEventCallback m_new_connection;
    ConnectionEventCallback m_connection_lost_callback;

    typedef std::unordered_map< int, Connection::Pointer > ConnectionContainer;
    ConnectionContainer m_connections;
};

}
}
