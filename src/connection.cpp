#include <thenet/connection.hpp>
#include <thenet/socket.hpp>
#include <thenet/types.hpp>

namespace the
{
namespace net
{

Connection::Connection( Socket& socket )
  : id( socket.id )
  , m_outgoing_packetizer( socket )
  , m_message_queue(
      std::bind(
        &packetizer::Outgoing::send,
        &m_outgoing_packetizer,
        std::placeholders::_1 ) )
  , m_incoming_packetizer( m_message_queue )
{
}


bool
Connection::send( Data&& data )
{
  return m_message_queue.send( std::move( data ) );
}


bool
Connection::receive( Data& data )
{
  return m_message_queue.receive( data );
}


void
Connection::data_from_network( const char* data, size_t length )
{
  m_incoming_packetizer.receive( data, length );
}


void
Connection::wake_up_on_network_thread()
{
  m_message_queue.wake_up();
}


}
}

