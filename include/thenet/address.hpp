#pragma once

#include <string>

namespace the
{
namespace net
{

class Address
{
  public:
    Address( const std::string& address );

  private:
    const size_t m_position_of_protocol_separator;
    const size_t m_start_of_host;
    const size_t m_position_of_port_separator;
    const size_t m_length_of_host;

  public:
    const std::string host;
    const int port;
    const std::string protocol;
};

}
}

