#include <igloo/igloo_alt.h>
using namespace igloo;

#include <thenet/socket.hpp>

namespace
{
  class TestSocket : public the::net::Socket
  {
    public:
      TestSocket( int fd )
        : the::net::Socket( fd )
      {
      }

      virtual void handle_event() override
      {
      }
  };

}

Describe( a_socket )
{

  It( has_a_unique_id )
  {
    TestSocket first_socket{ dummy_file_descriptor };
    TestSocket second_socket{ dummy_file_descriptor };
    AssertThat( first_socket.id, !Equals( second_socket.id ) );
  }

  const int dummy_file_descriptor{ 0 };
};

