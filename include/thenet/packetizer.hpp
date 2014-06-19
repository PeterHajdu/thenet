#pragma once

#include <functional>
#include <netinet/in.h>
#include <thenet/types.hpp>

namespace the
{
namespace net
{
class Socket;

namespace packetizer
{

template < typename UpperLayer >
class Incoming
{
  public:
    Incoming( UpperLayer& upper_layer )
      : m_upper_layer( upper_layer )
    {
    }


    void receive( const char* buffer, size_t length )
    {
      m_buffer.insert( std::end( m_buffer ), buffer, buffer + length );

      while( try_to_extract_message() )
      {
      }
    }


  private:
    bool try_to_extract_message()
    {
      const size_t end_of_message( parse_message() );
      if ( incomplete_message == end_of_message )
      {
        return false;
      }

      m_upper_layer.message_from_network(
          Data( &m_buffer[ header_length ], &m_buffer[ end_of_message ] ) );

      m_buffer.erase(
          std::begin( m_buffer ),
          std::begin( m_buffer ) + end_of_message );

      return true;
    }

    size_t parse_message()
    {
      if ( m_buffer.size() < header_length )
      {
        return incomplete_message;
      }

      const uint32_t message_length( ntohl( *( reinterpret_cast<uint32_t*>( &m_buffer[0] ) ) ) );

      if ( m_buffer.size() < message_length + header_length )
      {
        return incomplete_message;
      }

      return header_length + message_length;
    }


    //todo: consider other container, messages are going to be erased from the beginning of the buffer
    Data m_buffer;
    UpperLayer& m_upper_layer;
    static const size_t incomplete_message{ 0 };
    static const size_t header_length{ sizeof( uint32_t ) };
};

class Outgoing
{
  public:
    Outgoing( Socket& socket );
    void send( Data&& data );

  private:
    Socket& m_socket;
};

}
}
}

