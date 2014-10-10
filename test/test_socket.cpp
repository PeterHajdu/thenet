#include <igloo/igloo_alt.h>
using namespace igloo;

#include <thenet/socket.hpp>
#include <thenet/test_socket.hpp>

Describe( a_socket )
{

  It( has_a_unique_id )
  {
    test::Socket::Pointer first_socket{ test::Socket::create() };
    test::Socket::Pointer second_socket{ test::Socket::create() };
    AssertThat( first_socket->id, !Equals( second_socket->id ) );
  }

};

