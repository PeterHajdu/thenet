#include "test_message.hpp"
#include <thenet/packetizer.hpp>

namespace
{
  const std::string to_network( const std::string& plain )
  {
    const uint32_t network_message_length( htonl( plain.size() ) );
    const char* start_of_length_header( reinterpret_cast< const char* >( &network_message_length ) );
    const char* end_of_legth_header( start_of_length_header + sizeof( network_message_length ) );

    return std::string( start_of_length_header, end_of_legth_header ) + plain;
  }
}

namespace test
{
Message::Message( const std::string& message )
  : plain( message )
  , network( to_network( message ) )
  , plain_data( begin( message ), end( message ) )
  , network_data( begin( network ), end( network ) )
{
}


}

