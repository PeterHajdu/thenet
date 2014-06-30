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
        &Connection::send_on_network_thread,
        this,
        std::placeholders::_1 ) )
  , m_incoming_packetizer( *this )
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
Connection::message_from_network( Data&& message )
{
  for ( auto& task : m_tasks )
  {
    task->on_message_from_network( message );
  }

  m_message_queue.message_from_network( std::move( message ) );
}


void
Connection::wake_up_on_network_thread()
{
  for ( auto& task : m_tasks )
  {
    task->wake_up();
  }

  m_message_queue.wake_up();
}

void
Connection::send_on_network_thread( Data&& data )
{
  m_outgoing_packetizer.send( std::move( data ) );
}


void
Connection::register_task( NetworkTask::Pointer&& task )
{
  m_tasks.emplace_back( std::move( task ) );
}

}
}

