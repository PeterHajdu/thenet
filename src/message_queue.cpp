#include <thenet/message_queue.hpp>

namespace the
{

namespace net
{

MessageQueue::MessageQueue( LowLevelSendCallback low_level_send_callback )
  : m_low_level_send( low_level_send_callback )
{
}

bool
MessageQueue::send( Data&& data )
{
  return m_out_buffer.push( data );
}

bool
MessageQueue::receive( Data& data )
{
  return m_in_buffer.pop( data );
}

void
MessageQueue::wake_up()
{
  Data data;
  while ( m_out_buffer.pop( data ) )
  {
    m_low_level_send( std::move( data ) );
  }
}

void
MessageQueue::message_from_network( Data&& data )
{
  m_in_buffer.push( data );
}

}

}

