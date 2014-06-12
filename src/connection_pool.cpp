#include <thenet/connection_pool.hpp>
#include <thenet/socket.hpp>

namespace the
{
namespace net
{

ConnectionPool::ConnectionPool(
    ConnectionEventCallback new_connection_callback,
    ConnectionEventCallback connection_lost_callback )
  : m_new_connection( new_connection_callback )
  , m_connection_lost_callback( connection_lost_callback )
{
}

void
ConnectionPool::on_new_socket( Socket& socket )
{
  Connection::Pointer new_connection( new Connection( socket ) );
  m_new_connection( *new_connection );
  m_connections.emplace( std::make_pair(
        socket.id,
        std::move( new_connection ) ) );
}

void
ConnectionPool::on_socket_lost( Socket& socket )
{
  ConnectionContainer::iterator connection( m_connections.find( socket.id ) );
  if ( connection==m_connections.end() )
  {
    return;
  }

  m_connection_lost_callback( *connection->second );
  m_connections.erase( connection );
}

}
}

