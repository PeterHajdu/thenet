#include <igloo/igloo_alt.h>
using namespace igloo;

#include <thenet/socket.hpp>
#include "test_socket.hpp"

Describe( a_socket )
{

  It( has_a_unique_id )
  {
    test::Socket first_socket{ dummy_file_descriptor };
    test::Socket second_socket{ dummy_file_descriptor };
    AssertThat( first_socket.id, !Equals( second_socket.id ) );
  }

  const int dummy_file_descriptor{ 0 };
};

