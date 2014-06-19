#pragma once

#include <sys/types.h>
#include <unistd.h>
#include <array>

namespace the
{
namespace net
{

template < typename Owner >
class ReadingSocket : public Socket
{
  public:
    static const int messagebuffer_size{ 1000 };
    ReadingSocket( int socket, Owner& owner )
      : Socket( socket )
      , m_owner( owner )
    {
    }

    void handle_event() override
    {
      std::array< char, messagebuffer_size > buffer;
      ssize_t length( 0 );
      while (  0 < ( length = ::read( fd, &buffer[ 0 ], messagebuffer_size ) ) )
      {
        m_owner.on_data_available( *this, &buffer[ 0 ], length );
      }

      const bool connection_lost( !length );
      if ( connection_lost )
      {
        m_owner.on_socket_lost( *this );
        return;
      }
    }

  private:
    Owner& m_owner;
};

}
}

