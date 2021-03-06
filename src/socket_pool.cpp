#include <thenet/socket_pool.hpp>
#include <thenet/connection_pool.hpp>
#include <thenet/client_socket.hpp>
#include <thenet/socket_utils.hpp>
#include <thenet/address.hpp>

#include <unistd.h>
#include <string>
#include <algorithm>

namespace
{

  class ServerSocket : public the::net::Socket
  {
    public:
      ServerSocket( int port, the::net::SocketPool& socket_pool )
        : Socket( socket( AF_INET, SOCK_STREAM, 0 ) )
        , m_socket_pool( socket_pool )
      {
        the::net::allow_address_reuse( *this );
        the::net::bind_to_port( port, *this );
        the::net::listen_with_queue_length( 5, *this );
      }

      virtual void handle_event() override
      {
        struct sockaddr_in address;
        socklen_t sin_size = sizeof( address );

        Socket::Pointer new_socket( new the::net::ClientSocket<the::net::SocketPool>(
              accept(fd, (struct sockaddr *) &address, &sin_size),
              m_socket_pool ) );

        set_non_blocking( *new_socket );
        m_socket_pool.on_new_socket( std::move( new_socket ) );
      }

      virtual void drop() override
      {
      }

    private:
      the::net::SocketPool& m_socket_pool;
  };

}


namespace the
{
namespace net
{

SocketPool::SocketPool( ConnectionPool& connection_pool )
  : m_connection_pool( connection_pool )
{
}


void
SocketPool::listen( int port )
{
  add_socket( Socket::Pointer( new ServerSocket( port, *this ) ) );
}

void
SocketPool::on_data_available( Socket& socket, const char* data, size_t length )
{
  m_connection_pool.on_data_available( socket, data, length );
}

void
SocketPool::on_new_socket( Socket::Pointer&& socket )
{
  m_connection_pool.on_new_socket( *socket );
  add_socket( std::move( socket ) );
}


void
SocketPool::add_socket( Socket::Pointer&& socket )
{
  m_sockets_to_be_added.emplace_back( std::move( socket ) );
}


void
SocketPool::drop_socket( Socket& socket )
{
  m_sockets_to_be_dropped.emplace_back( socket );
}


void
SocketPool::on_socket_lost( Socket& socket )
{
  m_connection_pool.on_socket_lost( socket );

  auto new_end( std::remove_if( begin( m_poll_descriptors ), end( m_poll_descriptors ),
        [ &socket ] ( pollfd& poll_descriptor )
        {
          return poll_descriptor.fd == socket.fd;
        } ) );
  m_poll_descriptors.erase( new_end, end( m_poll_descriptors ) );
  m_sockets.erase( socket.fd );
}


void
SocketPool::clean_up_dropped_and_new_sockets()
{
  for ( auto& socket : m_sockets_to_be_dropped )
  {
    on_socket_lost( socket );
  }
  m_sockets_to_be_dropped.clear();

  for ( auto& socket : m_sockets_to_be_added )
  {
    m_poll_descriptors.emplace_back( pollfd{ socket->fd, POLLIN, 0 } );
    m_sockets.emplace( std::make_pair( socket->fd, std::move( socket ) ) );
  }
  m_sockets_to_be_added.clear();
}


void
SocketPool::run_for( uint32_t run_for_milliseconds )
{
  clean_up_dropped_and_new_sockets();

  if ( poll( &m_poll_descriptors[0], m_poll_descriptors.size(), run_for_milliseconds ) < 0 )
  {
    //todo: handle network error
    return;
  }

  for ( auto& poll_descriptor : m_poll_descriptors )
  {
    const bool no_event( !( poll_descriptor.revents & POLLIN) );
    if ( no_event )
    {
      continue;
    }

    m_sockets[ poll_descriptor.fd ]->handle_event();
  }

}


bool SocketPool::connect( const Address& address )
{
  const int new_socket( connect_socket( address ) );
  if ( new_socket < 0 )
  {
    return false;
  }

  Socket::Pointer connected_socket( new ClientSocket<SocketPool>( new_socket, *this ) );
  set_non_blocking( *connected_socket );
  on_new_socket( std::move( connected_socket ) );
  return true;
}

}
}

