#include <thenet/connection.hpp>
#include <thenet/socket.hpp>
#include <thenet/types.hpp>


namespace the
{
namespace net
{

Connection::Connection( Socket& socket )
  : id( socket.id )
  , m_message_queue(
      std::bind(
        &Socket::send,
        &socket,
        std::placeholders::_1,
        std::placeholders::_2 ) )
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
  m_message_queue.message_from_network( Data( data, data + length ) );
}


void
Connection::wake_up_on_network_thread()
{
  m_message_queue.wake_up();
}


}
}

