#include <igloo/igloo_alt.h>
using namespace igloo;

#include <thenet/connection_pool.hpp>
#include <thenet/socket.hpp>

#include "test_socket.hpp"

Describe( a_connection_pool )
{

  void SetUp()
  {
    connection_pool.reset( new the::net::ConnectionPool() );
  }

  It( creates_a_connection_for_each_new_socket_created )
  {
    connection_pool->on_new_socket( socket );
  }

  std::unique_ptr< the::net::ConnectionPool > connection_pool;
  const int dummy_file_descriptor{ 0 };
  test::Socket socket{ dummy_file_descriptor };
};

