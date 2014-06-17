#include <thenet/service.hpp>

namespace the
{
namespace net
{

Service::Service(
    ConnectionPool::ConnectionEventCallback on_new_connection,
    ConnectionPool::ConnectionEventCallback on_connection_lost )
  : m_connection_pool( on_new_connection, on_connection_lost )
  , m_socket_pool( m_connection_pool )
{
}

void
Service::start()
{
  m_network_thread.reset( new ThreadRaii(
      [ this ]()
      {
        const int ten_milliseconds( 10 );
        m_socket_pool.run_for( ten_milliseconds );
        m_connection_pool.wake_up_on_network_thread();
      } ) );
}


Service::~Service()
{
}


void
Service::listen_on( int port )
{
  m_socket_pool.listen( port );
}

void
Service::connect_to( const std::string& host, int port )
{
  m_socket_pool.connect( host, port );
}

}
}

