#include <igloo/igloo_alt.h>
using namespace igloo;

#include <thenet/connection.hpp>
#include "test_socket.hpp"

Describe( a_connection )
{

  It( clones_corresponding_sockets_id )
  {
    test::Socket socket( 0 );
    the::net::Connection connection( socket );
    AssertThat( socket.id, Equals( connection.id ) );
  }

};

