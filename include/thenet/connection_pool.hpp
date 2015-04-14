#pragma once

#include <functional>
#include <unordered_map>

#include <thenet/connection.hpp>
#include <mutex>

namespace the
{
namespace net
{
class Socket;

class ConnectionPool
{
  public:
    typedef std::function<void( Connection::Pointer )> ConnectionEventCallback;
    ConnectionPool(
        ConnectionEventCallback new_connection_callback,
        ConnectionEventCallback connection_lost_callback );

    void on_new_socket( Socket& new_socket );
    void on_socket_lost( Socket& lost_socket );
    void on_data_available( Socket& socket, const char* data, size_t length );

    void wake_up_on_network_thread() const;

    template <typename functor>
    void enumerate( functor ) const;

  private:
    ConnectionEventCallback m_new_connection;
    ConnectionEventCallback m_connection_lost_callback;

    typedef std::unordered_map< int, Connection::Pointer > ConnectionContainer;
    ConnectionContainer m_connections;
    mutable std::mutex m_connections_mutex;
};

template <typename functor>
void ConnectionPool::enumerate( functor enumerator ) const
{
  std::lock_guard<std::mutex> guard( m_connections_mutex );
  for ( auto& connection : m_connections )
  {
    enumerator( *(connection.second) );
  }
}


}
}

