#include <thenet/address.hpp>

namespace
{

const std::string protocol_separator( "://" );
const std::string port_separator( ":" );

size_t position_of_protocol_separator( const std::string& address )
{
  const size_t position_of_protocol_separator(
      address.find( protocol_separator ) );

  const bool protocol_separator_found( position_of_protocol_separator != std::string::npos );

  return
    protocol_separator_found ?
    position_of_protocol_separator:
    0;
}

size_t start_of_host( size_t position_of_protocol_separator )
{
  const bool protocol_separator_found( position_of_protocol_separator != 0 );

  return
    protocol_separator_found ?
    position_of_protocol_separator + protocol_separator.size() :
    0;
}

}

namespace the
{
namespace net
{


Address::Address( const std::string& address )
  : m_position_of_protocol_separator( position_of_protocol_separator( address ) )
  , m_start_of_host( start_of_host( m_position_of_protocol_separator ) )
  , m_position_of_port_separator( address.rfind( port_separator ) )
  , m_length_of_host( m_position_of_port_separator - m_start_of_host )
  , host( address.substr( m_start_of_host, m_length_of_host ) )
  , port( std::stoi(
      address.substr( m_position_of_port_separator + 1 ) ) )
  , protocol( address.substr( 0, m_position_of_protocol_separator ) )
{
}


}
}

