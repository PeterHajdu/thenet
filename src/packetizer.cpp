#include "packetizer.hpp"
#include <thenet/socket.hpp>

namespace the
{
namespace net
{
namespace packetizer
{

Outgoing::Outgoing( Socket& socket  )
  : m_socket( socket )
{
}

void
Outgoing::send( Data&& data )
{
  const uint32_t network_length_of_message( htonl( data.size() ) );
  m_socket.send(
      reinterpret_cast< const char* >( &network_length_of_message ),
      sizeof( network_length_of_message ) );
  m_socket.send( data.data(), data.size() );
}

}
}
}

