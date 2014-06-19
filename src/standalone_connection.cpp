#include <thenet/standalone_connection.hpp>
#include <thenet/connection.hpp>
#include <thenet/thread_raii.hpp>
#include <thenet/client_socket.hpp>
#include "socket_utils.hpp"


namespace
{
  std::unique_ptr< the::net::ThreadRaii >
  start_network_thread( the::net::Socket& socket )
  {
    return std::unique_ptr< the::net::ThreadRaii >(
        new the::net::ThreadRaii(
          [ &socket ] ()
          {
            socket.handle_event();
          } ) );
  }
}


namespace the
{
namespace net
{

StandaloneConnection::StandaloneConnection(
    const std::string& host,
    int port )
  : m_host( host )
  , m_port( port )
{
}

StandaloneConnection::~StandaloneConnection()
{
  m_socket->close();
}

void
StandaloneConnection::connect()
{
  const int connected_socket( connect_socket( m_host, m_port ) );
  if ( connected_socket < 0 )
  {
    return;
  }

  m_socket.reset(
      new ClientSocket< StandaloneConnection >(
        connected_socket,
        *this ) );

  m_connection.reset(
      new Connection(
        *m_socket ) );

  m_packetizer.reset(
      new packetizer::Outgoing(
        *m_socket ) );

  m_network_thread =
    std::move( start_network_thread( *m_socket ) );
}


void
StandaloneConnection::on_socket_lost( Socket& lost_socket )
{
}


void
StandaloneConnection::on_data_available( Socket& socket, const char* data, size_t length )
{
  m_connection->data_from_network( data, length );
}


bool
StandaloneConnection::send( Data&& data )
{
  m_packetizer->send( std::move( data ) );
  return true;
}


bool
StandaloneConnection::receive( Data& data )
{
  return m_connection->receive( data );
}

}
}

