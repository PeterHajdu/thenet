#include <thenet/connection.hpp>

namespace the
{
namespace net
{

  ConnectionBuffer::ConnectionBuffer(
      ParseMessageCallback parse_message_callback,
      MessageCompleteCallback message_complete_callback )
    : m_parse_message( parse_message_callback )
    , m_message_comlete( message_complete_callback )
  {
  }

  void
  ConnectionBuffer::receive( const char* buffer, size_t length )
  {
    m_buffer.insert( std::end( m_buffer ), buffer, buffer + length );
    const size_t end_of_message( m_parse_message( &m_buffer[0], m_buffer.size() ) );

    Data::const_iterator message_start( std::begin( m_buffer ) );
    Data::const_iterator message_end( std::begin( m_buffer ) + end_of_message );

    m_message_comlete( Data( message_start, message_end ) );
    m_buffer.erase( message_start, message_end );
  }

}
}

