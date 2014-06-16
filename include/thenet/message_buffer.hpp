#pragma once

#include <functional>

#include <thenet/types.hpp>

namespace the
{
namespace net
{

template < typename MessageParser, typename UpperLayer >
class MessageBuffer
{
  public:
    MessageBuffer( UpperLayer& upper_layer )
      : m_upper_layer( upper_layer )
    {
    }


    void receive( const char* buffer, size_t length )
    {
      m_buffer.insert( std::end( m_buffer ), buffer, buffer + length );
      const size_t end_of_message( MessageParser::parse( &m_buffer[0], m_buffer.size() ) );

      if ( 0 == end_of_message )
      {
        return;
      }

      Data::const_iterator message_start( std::begin( m_buffer ) );
      Data::const_iterator message_end( std::begin( m_buffer ) + end_of_message );

      m_upper_layer.message_from_network( Data( message_start, message_end ) );
      m_buffer.erase( message_start, message_end );
    }


  private:
    //todo: consider other container, messages are going to be erased from the beginning of the buffer
    Data m_buffer;
    UpperLayer& m_upper_layer;
};

}
}

