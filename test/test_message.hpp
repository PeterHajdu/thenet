#pragma once
#include <string>
#include <thenet/types.hpp>

namespace test
{
  class Message
  {
    public:
      Message( const std::string& message );
      const std::string plain;
      const std::string network;
      const the::net::Data plain_data;
      const the::net::Data network_data;
  };
}

