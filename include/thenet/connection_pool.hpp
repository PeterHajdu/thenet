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
    void on_data_available( Socket& lost_socket, const char* data, size_t length );

    template <typename functor>
    void enumerate( functor );

  private:
    ConnectionEventCallback m_new_connection;
    ConnectionEventCallback m_connection_lost_callback;

    typedef std::unordered_map< int, Connection::Pointer > ConnectionContainer;
    ConnectionContainer m_connections;
};

template <typename functor>
void ConnectionPool::enumerate( functor enumerator )
{
  for ( auto& connection : m_connections )
  {
    enumerator( *(connection.second) );
  }
}


}
}

