#include <thenet/connection.hpp>

namespace the
{

namespace net
{

Connection::Connection( LowLevelSendCallback low_level_send_callback )
  : m_low_level_send( low_level_send_callback )
{
}

bool
Connection::send( Data&& data )
{
  return m_out_buffer.push( data );
}

bool
Connection::receive( Data& data )
{
  return m_in_buffer.pop( data );
}

void
Connection::wake_up()
{
  Data data;
  while ( m_out_buffer.pop( data ) )
  {
    m_low_level_send( &data[0], data.size() );
  }
}

void
Connection::data_from_network( Data&& data )
{
  m_in_buffer.push( data );
}

}

}

