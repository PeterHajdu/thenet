#pragma once

#include <thenet/socket.hpp>

#include <sys/types.h>
#include <unistd.h>
#include <array>

namespace the
{
namespace net
{

template < typename Owner >
class ClientSocket : public Socket
{
  public:
    ClientSocket( int socket, Owner& owner )
      : Socket( socket )
      , m_owner( owner )
    {
    }

    void handle_event() override
    {
      ssize_t length( 0 );
      while (  0 < ( length = ::read( fd, &m_buffer[ 0 ], messagebuffer_size ) ) )
      {
        m_owner.on_data_available( *this, &m_buffer[ 0 ], length );
      }

      const bool connection_lost( !length );
      if ( connection_lost )
      {
        m_owner.on_socket_lost( *this );
        return;
      }
    }

    virtual void drop() override
    {
      m_owner.on_socket_lost( *this );
      return;
    }

  private:
    Owner& m_owner;
    static const int messagebuffer_size{ 1000 };
    std::array< char, messagebuffer_size > m_buffer;
};

}
}

