#include <thenet/socket_pool.hpp>
#include <unistd.h>
#include <netdb.h>
#include <array>
#include <string>
#include <algorithm>

namespace
{
  //todo: consider error handling of these system functions
  void allow_address_reuse( int socket )
  {
    const int true_value( 1 );
    setsockopt(
        socket,
        SOL_SOCKET,
        SO_REUSEADDR,
        &true_value,
        sizeof( true_value ) );
  }


  sockaddr_in create_base_sockaddr( int port )
  {
    struct sockaddr_in address;
    memset( &address, 0, sizeof( address ) );
    address.sin_family = AF_INET;
    address.sin_port = htons( port );
    return address;
  }


  void bind_to_port( int port, int socket )
  {
    struct sockaddr_in address( create_base_sockaddr( port ) );
    address.sin_addr.s_addr = INADDR_ANY;

    bind( socket, (struct sockaddr *) &address, sizeof( address ) );
  }


  void listen_with_queue_length( int queue_length, int socket )
  {
    ::listen( socket, queue_length );
  }


  const int messagebuffer_size( 1000 );
  class ReadingSocket : public the::net::Socket
  {
    public:
      ReadingSocket(
          int socket,
          the::net::SocketPool::ReadDataCallback read_data_callback,
          the::net::SocketPool::SocketEventCallback drop_socket_callback )
        : Socket( socket )
        , m_read_data_callback( read_data_callback )
        , m_drop_socket_callback( drop_socket_callback )
      {
      }

      void handle_event() override
      {
        std::array< char, messagebuffer_size > buffer;
        const size_t length( ::read( fd, &buffer[ 0 ], messagebuffer_size ) );

        const bool connection_lost( !length );
        if ( connection_lost )
        {
          m_drop_socket_callback( *this );
          return;
        }

        m_read_data_callback( *this, &buffer[ 0 ], length );
      }

    private:

      the::net::SocketPool::ReadDataCallback m_read_data_callback;
      the::net::SocketPool::SocketEventCallback m_drop_socket_callback;
  };


  class ListeningSocket : public the::net::Socket
  {
    public:
      typedef std::function<void(the::net::Socket::Pointer&&)> AddSocketCallback;
      ListeningSocket(
          int port,
          AddSocketCallback add_socket_callback,
          the::net::SocketPool::ReadDataCallback read_data_callback,
          the::net::SocketPool::SocketEventCallback drop_socket_callback )
        : Socket( socket( AF_INET, SOCK_STREAM, 0 ) )
        , m_add_socket_callback( add_socket_callback )
        , m_read_data_callback( read_data_callback )
        , m_drop_socket_callback( drop_socket_callback )
      {
        allow_address_reuse( fd );
        bind_to_port( port, fd );
        listen_with_queue_length( 5, fd );
      }

      virtual void handle_event() override
      {
        struct sockaddr_in address;
        socklen_t sin_size = sizeof( address );

        Socket::Pointer new_socket( new ReadingSocket(
              accept(fd, (struct sockaddr *) &address, &sin_size),
              m_read_data_callback,
              m_drop_socket_callback ) );

        m_add_socket_callback( std::move( new_socket ) );
      }


    private:
      AddSocketCallback m_add_socket_callback;
      the::net::SocketPool::ReadDataCallback m_read_data_callback;
      the::net::SocketPool::SocketEventCallback m_drop_socket_callback;
  };

}


namespace the
{
namespace net
{

SocketPool::SocketPool(
    SocketEventCallback new_socket,
    SocketEventCallback drop_socket,
    ReadDataCallback read_data )
  : m_new_socket_callback( new_socket )
  , m_drop_socket_callback( drop_socket )
  , m_read_data_callback( read_data )
{
}


void
SocketPool::listen( int port )
{
  add_socket( Socket::Pointer(
        new ListeningSocket(
          port,
          std::bind( &SocketPool::add_socket_with_callback, this, std::placeholders::_1 ),
          m_read_data_callback,
          std::bind( &SocketPool::drop_socket, this, std::placeholders::_1 ) ) ) );
}


void
SocketPool::add_socket_with_callback( Socket::Pointer&& socket )
{
  m_new_socket_callback( *socket );
  add_socket( std::move( socket ) );
}


void
SocketPool::add_socket( Socket::Pointer&& socket )
{
  m_poll_descriptors.emplace_back( pollfd{ socket->fd, POLLIN, 0 } );
  m_sockets.emplace( std::make_pair( socket->fd, std::move( socket ) ) );
}


void
SocketPool::drop_socket( Socket& socket )
{
  m_drop_socket_callback( socket );

  auto new_end( std::remove_if( begin( m_poll_descriptors ), end( m_poll_descriptors ),
        [ &socket ] ( pollfd& poll_descriptor )
        {
          return poll_descriptor.fd == socket.fd;
        } ) );
  m_poll_descriptors.erase( new_end, end( m_poll_descriptors ) );
  m_sockets.erase( socket.fd );
}


void
SocketPool::run_for( uint32_t run_for_milliseconds )
{
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


bool SocketPool::connect( const std::string& address, int port )
{
  struct hostent *serverHost( gethostbyname( address.c_str() ) );

  struct sockaddr_in serverData( create_base_sockaddr( port ) );
  serverData.sin_family = AF_INET;

  memcpy( serverHost->h_addr, &(serverData.sin_addr.s_addr), serverHost->h_length);

  Socket::Pointer new_socket( new ReadingSocket(
        socket( PF_INET, SOCK_STREAM, IPPROTO_TCP ),
        m_read_data_callback,
        std::bind( &SocketPool::drop_socket, this, std::placeholders::_1 ) ) );

  if ( ::connect( new_socket->fd, (struct sockaddr *)&serverData, sizeof( serverData ) ) < 0 )
  {
    return false;
  }

  add_socket_with_callback( std::move( new_socket ) );
  return true;
}

}
}

